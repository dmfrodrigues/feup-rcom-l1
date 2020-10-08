#include "application_layer.h"

application_layer app;

int application(int port_fd, ll_status_t status, int baud_rate, unsigned int timeout, unsigned int retransmissions){

    app.fileDescriptor = port_fd;
    app.status = status;
    app.ll_config.baud_rate = baud_rate;
    app.ll_config.timeout = timeout;
    app.ll_config.retransmissions = retransmissions;
    
    llopen(app.fileDescriptor , app.status);

    if(app.status == TRANSMITTER){
        app_send_data();
    }else{
        app_receive_data();
    }

    return -1;
}

int app_send_ctrl_packet(int ctrl, size_t file_size, char *file_name){
    
    size_t packet_size = 5 + sizeof(size_t) + strlen(file_name);
    uint8_t *ctrl_packet = (uint8_t *) malloc(packet_size);

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
    for(int i = 0; i < strlen(file_name); i++){
        ctrl_packet[i+9] = file_name[i];
    }
    
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
    uint8_t *data_packet = (uint8_t*) malloc(packet_size);

    data_packet[0] = CTRL_DATA;
    data_packet[1] = seq_number % 255;

    // data_size = 256*L2 + L1 
    // is it right?
    data_packet[2] = (data_size & 0xFF00);    //L2
    data_packet[3] = (data_size & 0xFF);      //L1

    for(int i = 0; i < data_size; i++){
        data_packet[i+4] = data[i];
    }

    if(llwrite(app.fileDescriptor, data_packet, packet_size) < 0){
        fprintf(stderr, "ERROR: unable to write data packet\n");
        free(data_packet);
        return -1;
    }

    free(data_packet);
    
    return 1;
}

int app_send_data(){
    // TODO
    return -1;
}

int app_receive_data(){
    // TODO
    return -1;
}

