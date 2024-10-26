#include "lights.h"
#include "signal_handler.h"
#include "socket_io.h"
#include "tcp_server.h"
#include "utilities.h"

#include <unistd.h>

#if defined(__aarch64__) || defined(__arm__)
#include <bcm2835.h> // Include only on ARM-based systems (Raspberry Pi)
#endif

#define MAX_BUFF_SIZE    14
#define PLACEHOLDER_NUM  10
#define BCM_INIT_FAILURE 0

int lights_init()
{
    int exit_code = E_FAILURE;

#if defined(__aarch64__) || defined(__arm__)
    if (!bcm2835_init())
    {
        print_error("lights_init(): Failed to initialize bcm2835.");
        goto END;
    }

    // Set GPIO pins as outputs
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_40, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_38, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_36, BCM2835_GPIO_FSEL_OUTP);

#else
    printf("lights_init(): Running on non-ARM system. Skipping GPIO setup.\n");
#endif

    exit_code = E_SUCCESS;
    // END:
    return exit_code;
}

int lights_destroy()
{
    int exit_code = E_FAILURE;

#if defined(__aarch64__) || defined(__arm__)
    // Ensure LEDs are off
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_40, LOW);
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_38, LOW);
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_36, LOW);

    if (!bcm2835_close())
    {
        print_error("lights_destroy(): Failed to properly close bcm2835.");
        goto END;
    }

#else
    printf(
        "lights_destroy(): Running on non-ARM system. Skipping GPIO "
        "cleanup.\n");
#endif

    exit_code = E_SUCCESS;
    // END:
    return exit_code;
}

int light_green_on()
{
    int exit_code = E_FAILURE;

#if defined(__aarch64__) || defined(__arm__)
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_40, HIGH);
#else
    printf("light_green_on(): Green LED is on.\n");
#endif

    exit_code = E_SUCCESS;
    return exit_code;
}

int light_green_off()
{
    int exit_code = E_FAILURE;

#if defined(__aarch64__) || defined(__arm__)
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_40, LOW);
#else
    printf("light_green_off(): Green LED is off.\n");
#endif

    exit_code = E_SUCCESS;
    return exit_code;
}

int light_red_on()
{
    int exit_code = E_FAILURE;

#if defined(__aarch64__) || defined(__arm__)
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_38, HIGH);
#else
    printf("light_red_on(): Red LED is on.\n");
#endif

    exit_code = E_SUCCESS;
    return exit_code;
}

int light_red_off()
{
    int exit_code = E_FAILURE;

#if defined(__aarch64__) || defined(__arm__)
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_38, LOW);
#else
    printf("light_red_off(): Red LED is off.\n");
#endif

    exit_code = E_SUCCESS;
    return exit_code;
}

int light_yellow_on()
{
    int exit_code = E_FAILURE;

#if defined(__aarch64__) || defined(__arm__)
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_36, HIGH);
#else
    printf("light_yellow_on(): Yellow LED is on.\n");
#endif

    exit_code = E_SUCCESS;
    return exit_code;
}

int light_yellow_off()
{
    int exit_code = E_FAILURE;

#if defined(__aarch64__) || defined(__arm__)
    bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_36, LOW);
#else
    printf("light_yellow_off(): Yellow LED is off.\n");
#endif

    exit_code = E_SUCCESS;
    return exit_code;
}

/*** end of file ***/
