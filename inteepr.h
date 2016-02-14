#ifndef    INTEEPR_H
#define    INTEEPR_H

unsigned int IntEeprWordRead (unsigned int eepr_adr);
unsigned long IntEeprDwordRead (unsigned int eepr_adr);

void IntEeprWordWrite (unsigned int eepr_adr, unsigned int eepr_data);
void IntEeprDwordWrite (unsigned int eepr_adr, unsigned long err_data);

void IntEeprBlockRead (unsigned int buff_adr, unsigned int eepr_adr, unsigned char size);

void IntEeprBlockWrite (unsigned int buff_adr, unsigned int eepr_adr, unsigned char size);

#endif    /* INTEEPR_H */
