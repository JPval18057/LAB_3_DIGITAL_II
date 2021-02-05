//Libreria para usar el ADC
// Autor: Juan Pablo Valenzuela
// carnet: 18057
// Notas: Originalmente la iba a hacer en ingles porque mi MPLABX está en ingles
// por eso es que se pueden encontrar partes en inglés y partes en español
// aunque si es muy posible que la haya hecho en español para que no parezca que la copie
// decidí traducirla porque no tiene mucho sentido que esté en ingles para esta clase
// En futuras ocasiones voy a hacer mis librerías en español

#define _XTAL_FREQ 8000000      //I am using the internal 8MHz 

//Funcion para leer un dato analógico
void read_value(void){
    //Aca se lee el valor de voltaje analógico
    //antes de leer hay que esperar por lo menos 5us
    __delay_us(10);
    ADCON0bits.GO_DONE = 1; //start ADC operation
    //the value from the conversion is stored into a variable in the interruption
    return;
}

void ADC_config(void){
	//ADC INTERRUPTS
    ADCON0 = 0b10000001; //ACTIVATE THE ADC
    ADCON1 = 0b01001111; //left justified result with ref on vcc and gnd
    PIE1bits.ADIE = 1; //ACTIVATE ADC INTERRUPTION
    PIR1bits.ADIF = 0; //SHUT DOWN THE FLAG
	TRISA = 255; //PORTA AS AN INPUT
    ANSEL = 1; //PORTA PIN 0 AS AN ANALOG INPUT, THE REST AS A DIGITAL INPUT
    ANSELH = 0; //THE REST OF THE CHANNELS ARE DIGITAL
	return;	
}


