#include "application_layer.h"

application_layer app;

int application(int com, ll_status_t status, const char *file_name){

    app.status = status;
    app.fileDescriptor = llopen(com, app.status);

    if(app.status == TRANSMITTER){
        app_send_file(file_name);
    }else{
        app_receive_file();
    }

    if(llclose(app.fileDescriptor) < 0)
        return -1;

    return 1;
}

int app_send_ctrl_packet(int ctrl, size_t file_size, const char *file_name){
    
    size_t packet_size = 5 + sizeof(size_t) + strlen(file_name);
    char *ctrl_packet = (char *) malloc(packet_size);

    ctrl_packet[0] = ctrl;

    ctrl_packet[1] = T_FILE_SIZE;    // T1
    ctrl_packet[2] = sizeof(size_t); // L1 -> 4 octets
    // V1
    ctrl_packet[3] = (file_size & 0xFF);
    ctrl_packet[4] = (file_size & 0xFF00);  
    ctrl_packet[5] = (file_size & 0xFF0000);  
    ctrl_packet[6] = (file_size & 0xFF000000);  
    
    ctrl_packet[7] = T_FILE_NAME;       // T2
    ctrl_packet[8] = strlen(file_name); // L2
    
    // V2
    memcpy(ctrl_packet + 9, file_name, strlen(file_name));
    
    
    if(llwrite(app.fileDescriptor, ctrl_packet, packet_size) < 0){
        fprintf(stderr, "ERROR: unable to write control packet\n");
        free(ctrl_packet);
        return -1;
    }

    free(ctrl_packet);

    return 1;
}


int app_send_data_packet(uint8_t *data, size_t data_size, unsigned int seq_number){

    size_t packet_size = 4 + data_size;
    char *data_packet = (char*) malloc(packet_size);

    data_packet[0] = CTRL_DATA;
    data_packet[1] = seq_number % 255;

    // data_size = 256*L2 + L1 
    data_packet[2] = (data_size & 0xFF00);    //L2
    data_packet[3] = (data_size & 0xFF);      //L1

    memcpy(data_packet + 4, data, data_size);

    if(llwrite(app.fileDescriptor, data_packet, packet_size) < 0){
        fprintf(stderr, "ERROR: unable to write data packet\n");
        free(data_packet);
        return -1;
    }

    free(data_packet);
    
    return 1;
}

int app_send_file(const char *file_name){

    struct stat st;
    stat(file_name, &st);
    size_t file_size = st.st_size;

    if(app_send_ctrl_packet(CTRL_START, file_size, file_name) < 0)
        return -1;

    unsigned int seq_number = 0;

    FILE *file = fopen(file_name, "r");

    uint8_t *buf =(uint8_t*)malloc(LL_MAX_SIZE);

    while(fread(buf, sizeof(uint8_t), LL_MAX_SIZE, file) > 0){
        app_send_data_packet(buf, file_size, seq_number);
        seq_number++;
    }

    if(app_send_ctrl_packet(CTRL_END, file_size, file_name) < 0)
        return -1;

    fclose(file);

    return 1;
}

int app_rcv_ctrl_packet(int ctrl, int * file_size, char * file_name){

    char * ctrl_packet = (char*)malloc(5 + sizeof(size_t) + FILE_NAME_MAX_SIZE);

    if(llread(app.fileDescriptor, ctrl_packet) < 0){
        fprintf(stderr, "ERROR: unable to read ctrl packet\n");
        return -1;
    }

    if(ctrl_packet[0] != ctrl || ctrl_packet[1] != T_FILE_SIZE){
        fprintf(stderr, "ERROR: control is not correct\n");
        return -1;
    }
    
    int file_size_octets = ctrl_packet[2];
    
    memcpy(file_size, ctrl_packet + 3, file_size_octets);

    if(ctrl_packet[7] != T_FILE_NAME){
        fprintf(stderr, "ERROR: unable to read ctrl packet\n");
        return -1;
    }

    int file_name_length = ctrl_packet[8];


    memcpy(file_name, ctrl_packet + 9, file_name_length);

    free(ctrl_packet);

    return 1;
}

int app_rcv_data_packet(uint8_t * data, int seq_number){

    char * data_packet = (char*) malloc(LL_MAX_SIZE);
    
    if(llread(app.fileDescriptor, data_packet) < 0){
        fprintf(stderr, "ERROR: unable to read data packet\n");
        return -1;
    }

    if(data_packet[0] != CTRL_DATA){
        fprintf(stderr, "ERROR: data control is not correct\n");
        return -1;
    }

    if(data_packet[1] != seq_number){
        fprintf(stderr, "ERROR: sequence number is not correct\n");
        return -1;
    }

    int data_length = 256*data_packet[2] + data_packet[3];

    memcpy(data, data_packet + 4, data_length);

    free(data_packet);

    return data_length;
}

int app_receive_file(){
    
    int file_size;
    char *file_name = (char*) malloc(FILE_NAME_MAX_SIZE);

    if(app_rcv_ctrl_packet(CTRL_START, &file_size, file_name) < 0){
        return -1;
    }

    FILE *file = fopen(file_name, "w");

    int data_bytes_read = 0;
    unsigned int seq_number = 0;
    uint8_t *buf =(uint8_t*)malloc(LL_MAX_SIZE);

    while(file_size > data_bytes_read){
        
        int data_length = app_rcv_data_packet(buf, seq_number);
        fwrite(buf, sizeof(uint8_t), data_length, file);
        data_bytes_read += data_length;
        seq_number++;
    }

    fclose(file);

    int file_size_;
    char *file_name_ = (char*) malloc(FILE_NAME_MAX_SIZE);

    if(app_rcv_ctrl_packet(CTRL_END, &file_size_, file_name_) < 0){
        return -1;
    }

    if(file_size != file_size_ || file_name != file_name_){
        return -1;
    }

    return 1;
}
