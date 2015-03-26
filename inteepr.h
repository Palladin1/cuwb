#ifndef INTEEPR_H
#define INTEEPR_H

void IntEeprWordWrite (unsigned int eepr_adr, unsigned int eepr_data);
void IntEeprDwordWrite (unsigned int eepr_adr, unsigned long err_data);


unsigned int IntEeprWrldRead(unsigned int eepr_adr);
unsigned long IntEeprDwordRead(unsigned int eepr_adr);

void IntEeprBlockRead(unsigned int buff_adr, unsigned int eepr_adr, unsigned char size);

#endif
