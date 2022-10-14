/* kernel/port.h
 *
 * Interface to read and write to port.
 *
 * created: 2022/10/14 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/14 - xlmod <glafond-@student.42.fr>
 */

#ifndef PORT_H
#define PORT_H

#include <stdint.h>

uint8_t		port_read(uint16_t port);
void		port_write(uint16_t port, uint8_t val);

#endif
