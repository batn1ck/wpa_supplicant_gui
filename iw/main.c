#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/wireless.h>
#include "interfaces.h"

int main(void)
{
    struct iw_info *interfaces, *temp;
    interfaces = get_iw_info();
    temp = interfaces;

    while ( temp != NULL ) {
        printf("%s Mac: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", temp->iw_name, temp->mac_addr[0], temp->mac_addr[1], temp->mac_addr[2], temp->mac_addr[3], temp->mac_addr[4], temp->mac_addr[5]);
        temp = temp->iw_next;
    }

    clear_iw_list(interfaces);
    return 0;
}
