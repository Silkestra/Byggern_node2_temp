
#include "sam.h"
#include "can.h"
#include <stdio.h>

void can_printmsg(CanMsg m){
    printf("CanMsg(id:%d, length:%d, data:{", m.id, m.length);
    if(m.length){
        printf("%d", m.byte[0]);
    }
    for(uint8_t i = 1; i < m.length; i++){
        printf(", %d", m.byte[i]);
    }
    printf("})\n");
}


#define txMailbox 0
#define rxMailbox 1



void can_tx(CanMsg m){
    while(!(CAN0->CAN_MB[txMailbox].CAN_MSR & CAN_MSR_MRDY)){}
    
    // Set message ID and use CAN 2.0B protocol
    CAN0->CAN_MB[txMailbox].CAN_MID = CAN_MID_MIDvA(m.id) | CAN_MID_MIDE ;
        
    // Coerce maximum 8 byte length
    m.length = m.length > 8 ? 8 : m.length;
    
    //  Put message in can data registers
    CAN0->CAN_MB[txMailbox].CAN_MDL = m.dword[0];
    CAN0->CAN_MB[txMailbox].CAN_MDH = m.dword[1];
        
    // Set message length and mailbox ready to send
    CAN0->CAN_MB[txMailbox].CAN_MCR = (m.length << CAN_MCR_MDLC_Pos) | CAN_MCR_MTCR;
}

uint8_t can_rx(CanMsg* m){
    if(!(CAN0->CAN_MB[rxMailbox].CAN_MSR & CAN_MSR_MRDY)){
        return 0;
    }

    // Get message ID
    m->id = (uint8_t)((CAN0->CAN_MB[rxMailbox].CAN_MID & CAN_MID_MIDvA_Msk) >> CAN_MID_MIDvA_Pos);
        
    // Get data length
    m->length = (uint8_t)((CAN0->CAN_MB[rxMailbox].CAN_MSR & CAN_MSR_MDLC_Msk) >> CAN_MSR_MDLC_Pos);
    
    // Get data from CAN mailbox
    m->dword[0] = CAN0->CAN_MB[rxMailbox].CAN_MDL;
    m->dword[1] = CAN0->CAN_MB[rxMailbox].CAN_MDH;
                
    // Reset for new receive
    CAN0->CAN_MB[rxMailbox].CAN_MMR = CAN_MMR_MOT_MB_RX;
    CAN0->CAN_MB[rxMailbox].CAN_MCR |= CAN_MCR_MTCR;
    return 1;
}
    
    

    
/*
// Example CAN interrupt handler
void CAN0_Handler(void){
    char can_sr = CAN0->CAN_SR; 
    
    // RX interrupt
    if(can_sr & (1 << rxMailbox)){
        // Add your message-handling code here
        can_printmsg(can_rx());
    } else {
        printf("CAN0 message arrived in non-used mailbox\n\r");
    }
    
    if(can_sr & CAN_SR_MB0){
        // Disable interrupt
        CAN0->CAN_IDR = CAN_IER_MB0;
    }
    
    NVIC_ClearPendingIRQ(ID_CAN0);
} 
*/

