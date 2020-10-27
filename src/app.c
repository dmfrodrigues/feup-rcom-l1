// Copyright (C) 2020 Diogo Rodrigues, Breno Pimentel
// Distributed under the terms of the GNU General Public License, version 3

#include "app.h"
#include <time.h>
#include <limits.h>

#include "stats.h"

#include "ll_utils.h"

app_config_t app_config = {
    -1,
    LL_MAX_SIZE
};

int application(int com, ll_status_t status, char *file_path){

    app_config.fileDescriptor = llopen(com, status);
    if(app_config.fileDescriptor == -1) return -1;

    if(status == TRANSMITTER){
        if(app_send_file(file_path) < 0){
            fprintf(stderr, "ERROR: unable to send file\n");
            return -1;
        }
    }else{
        if(app_receive_file() < 0){
            fprintf(stderr, "ERROR: unexpected error receiving file\n");
            return -1;
        }
    }

    if(llclose(app_config.fileDescriptor) < 0)
        return -1;

    return 0;
}

int app_send_ctrl_packet(int ctrl, uint32_t file_size, const char *file_name){
    if(ctrl == CTRL_START) ADD_FILE_LENGTH(file_size);
    
    uint32_t packet_size = 5 + sizeof(uint32_t) + strlen(file_name);
    uint8_t *ctrl_packet = (uint8_t *) malloc(packet_size);

    ctrl_packet[0] = ctrl;

    ctrl_packet[1] = T_FILE_SIZE;    // T1
    ctrl_packet[2] = sizeof(uint32_t); // L1
    // V1
    ctrl_packet[3] = (file_size & 0xFF);
    ctrl_packet[4] = (file_size & 0xFF00) >> 8;  
    ctrl_packet[5] = (file_size & 0xFF0000) >> 16;  
    ctrl_packet[6] = (file_size & 0xFF000000) >> 24;  

    
    ctrl_packet[7] = T_FILE_NAME;       // T2
    ctrl_packet[8] = strlen(file_name); // L2
    
    // V2
    memcpy(ctrl_packet + 9, file_name, strlen(file_name));
    
    if(llwrite(app_config.fileDescriptor, ctrl_packet, packet_size) < 0){
        fprintf(stderr, "ERROR: unable to write control packet\n");
        free(ctrl_packet);
        return -1;
    }

    free(ctrl_packet);

    return 0;
}


int app_send_data_packet(char *data, unsigned int data_size, unsigned int seq_number){

    unsigned int packet_size = 4 + data_size;
    uint8_t *data_packet = (uint8_t*) malloc(packet_size);

    data_packet[0] = CTRL_DATA;
    data_packet[1] = seq_number % 255;

    // data_size = 256*L2 + L1 
    data_packet[2] = (data_size & 0xFF00) >> 8;    //L2
    data_packet[3] = (data_size & 0xFF);      //L1

    memcpy(data_packet + 4, data, data_size);

    if(llwrite(app_config.fileDescriptor, data_packet, packet_size) < 0){
        fprintf(stderr, "ERROR: unable to write data packet\n");
        free(data_packet);
        return -1;
    }

    free(data_packet);
    
    return 0;
}

int app_send_file(char *file_path){

    FILE *file = fopen(file_path, "rb");

    if(file == NULL) {
		printf("ERROR: error opening file %s\n", file_path);
		return -1;
	}

    struct stat st;
    stat(file_path, &st);
    uint32_t file_size = st.st_size;

    char *file_name = basename(file_path) + '\0';

    if(app_send_ctrl_packet(CTRL_START, file_size, file_name) < 0)
        return -1;

    char *buf = (char*)malloc(app_config.packet_size);

    unsigned int res = 0;
    unsigned int seq_number = 0;

    while((res = fread(buf, sizeof(char), app_config.packet_size, file)) > 0){
        if(app_send_data_packet(buf, res, seq_number) < 0)
            return -1;
        seq_number = (seq_number+1)%255;
    }

    if(app_send_ctrl_packet(CTRL_END, file_size, file_name) < 0)
        return -1;

    fclose(file);
    free(buf);

    return 0;
}

int app_rcv_ctrl_packet(int ctrl, unsigned int * file_size, char * file_name){
    ll_log(2, "APP: preparing to read ctrl packet\n");

    uint8_t *ctrl_packet = (uint8_t*)malloc(5+sizeof(unsigned int)+NAME_MAX);

    if(llread(app_config.fileDescriptor, ctrl_packet) < 0){
        fprintf(stderr, "ERROR: unable to read ctrl packet\n");
        free(ctrl_packet);
        return -1;
    }

    if(ctrl_packet[0] != ctrl || ctrl_packet[1] != T_FILE_SIZE){
        fprintf(stderr, "ERROR: control is not correct\n");
        free(ctrl_packet);
        return -1;
    }
    
    int file_size_octets = ctrl_packet[2];
    
    if(file_size_octets > 4){
        ll_err("ERROR: file_size_octets > 4\n");
        free(ctrl_packet);
        return -1;
    }
    
    memcpy(file_size, ctrl_packet + 3, file_size_octets);

    if(ctrl_packet[7] != T_FILE_NAME){
        fprintf(stderr, "ERROR: unexpected byte in control packet\n");
        free(ctrl_packet);
        return -1;
    }

    int file_name_length = ctrl_packet[8];

    int i=0;
    for(; i<file_name_length; i++)   {
        file_name[i] = (char)ctrl_packet[9+i];
    }
    file_name[i] = '\0';

    ll_log(2, "About to free ctrl_packet\n");
    free(ctrl_packet);

    ll_log(2, "APP: successfully read ctrl packet\n");

    return 0;
}

int app_rcv_data_packet(char * data, int seq_number){
    ll_log(2, "APP: preparing to read data packet\n");

    uint8_t * data_packet = (uint8_t*) malloc(app_config.packet_size + 4);
    
    if(llread(app_config.fileDescriptor, data_packet) < 0){
        fprintf(stderr, "ERROR: unable to read data packet\n");
        free(data_packet);
        return -1;
    }

    if(data_packet[0] != CTRL_DATA){
        ll_err("ERROR: data control is not correct\n"
               "data control=0x%02X (should be 0x%02X)\n",
               data_packet[0], CTRL_DATA);
        free(data_packet);
        return -1;
    }

    if(data_packet[1] != seq_number){
        ll_err("ERROR: sequence number is not correct\n");
        free(data_packet);
        return -1;
    }

    size_t data_length = 256*data_packet[2] + data_packet[3];
    
    if(data_length > app_config.packet_size){
        ll_err("ERROR: transmitted data length %d is larger than packet size %d\n",
               data_length, app_config.packet_size);
        free(data_packet);
        return -1;
    }

    memcpy(data, data_packet + 4, data_length);

    ll_log(2, "About to free data_packet, data_length=%lu, app_config.packet_size=%lu\n", data_length, app_config.packet_size);

    free(data_packet);

    ll_log(2, "APP: successfully read data packet\n");

    return data_length;
}

int app_receive_file(){
    
    unsigned int file_size;
    char *file_name = (char*) malloc(NAME_MAX);

    if(app_rcv_ctrl_packet(CTRL_START, &file_size, file_name) < 0){
        return -1;
    }

    FILE *file = fopen(file_name, "wb");

    if(file == NULL) {
		printf("ERROR: error opening file %s\n", file_name);
		return -1;
	}

    int data_length;
    unsigned int data_bytes_read = 0;
    unsigned int seq_number = 0;
    char *buf =(char*)malloc(app_config.packet_size);
    
    while(file_size > data_bytes_read){

        data_length = app_rcv_data_packet(buf, seq_number);
        
        if(data_length < 0){
            fprintf(stderr, "ERROR: app_rcv_data_packet\n");
            return -1;
        }

        fwrite(buf, sizeof(uint8_t), data_length, file);

        data_bytes_read += data_length;
        seq_number++;
    }

    free(buf);

    fclose(file);

    unsigned int file_size_;
    char *file_name_ = (char*) malloc(NAME_MAX);

    if(app_rcv_ctrl_packet(CTRL_END, &file_size_, file_name_) < 0){
        return -1;
    }

    if(file_size != file_size_ || strcmp(file_name, file_name_) != 0){
        fprintf(stderr, "ERROR: Start control packet filename: "
                        "%s differs from End control packet filename: %s\n",
                        file_name, file_name_);
        return -1;
    }

    free(file_name);
    free(file_name_);

    return 0;
}
