#include <xc.h>

// Configuration bits
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// Define the clock frequency
#define _XTAL_FREQ 20000000  // 20 MHz crystal oscillator

// Function Prototypes
void ADC_Init();
unsigned int ADC_Read(unsigned char channel);
float Voltage_Calculation(unsigned int adc_value);

void main() {
    unsigned int adc_value;
    float voltage;

    // Initialize the ADC module
    ADC_Init();

    // Main loop
    while (1) {
        // Read the ADC value from channel 0 (AN0)
        adc_value = ADC_Read(0);
        
        // Calculate the input voltage
        voltage = Voltage_Calculation(adc_value);

        // Add your code here to display the voltage (e.g., send via UART, display on LCD, etc.)
        // For this example, we will use a simple infinite loop
        __delay_ms(1000);  // Wait for 1 second before next reading
    }
}

void ADC_Init() {
    ADCON0 = 0x41; // ADCON0: ADON = 1 (ADC enabled), CHS = 000 (AN0), ADCS = 01 (Fosc/8)
    ADCON1 = 0x80; // ADCON1: ADFM = 1 (right justified), PCFG3:PCFG0 = 1000 (AN0 is analog input, others are digital)
}

unsigned int ADC_Read(unsigned char channel) {
    // Select ADC channel
    ADCON0 &= 0xC7;  // Clear current channel selection
    ADCON0 |= (channel << 3);  // Set the desired channel

    // Wait for acquisition time
    __delay_us(30);

    // Start the conversion
    ADCON0bits.GO_nDONE = 1;

    // Wait for the conversion to finish
    while (ADCON0bits.GO_nDONE);

    // Return the result (10-bit ADC value)
    return ((ADRESH << 8) + ADRESL);
}

float Voltage_Calculation(unsigned int adc_value) {
    // ADC result is 10-bit (0-1023), reference voltage is 5V
    float voltage = (adc_value * 5.0) / 1023.0;

    // Compensate for the voltage divider
    voltage = voltage * 21.0;  // Voltage divider ratio is 21 (100V/4.76V)

    return voltage;
}
