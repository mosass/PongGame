/*
********************************************* 
*
* Chip: MAX7219
* Author: Phanomphon Yotchon (TheMosass)
*
********************************************* 
*/

// PIN Configuration // 
#define CLK       PIN_D2 
#define LOAD      PIN_D3 
#define SEND_DATA PIN_D4 

// - Mode Selection 
#define decode 0x09 
#define brightness 0x0A 
#define scanLimit 0x0B 
#define shutDown 0x0C 
#define dispTest 0x0F 

// Firt 4 bites (not used generally) 
#define firstBites 0x0

// Wait function 
#define wait delay_ms(1)

int16 serialData=0; 



void clock_tick() // clock (CLK) pulse 
{ 
   output_low(CLK); 
   wait; 
   output_high(CLK); 
} 

void load_tick()  // load (LOAD) pulse 
{ 
   output_low(LOAD); 
   wait; 
   output_high(LOAD); 
} 

void transfer_data(int16 data) // Transfer 16 bit data word to the MAX7219 
{ 
   int count; 
   for (count = 0; count < 16; ++count) 
   { 
      output_bit(SEND_DATA, shift_left(&data,2,0)); // set data (DIN) level 
      clock_tick(); // clock data in 
   } 
   load_tick(); // latch the last 16 bits clocked 
}

void data_packing(byte mode, int value) // Standart data package function 
{ 
   serialData = 0;  
   serialData |= mode; 
   serialData <<= 8; 
   serialData |= value; 
   transfer_data(serialData);
}

void write_data(int digit, int data) // Send data to digits 
{
   data_packing(digit, data); 
}

void shutdown_7219(int operatingMode) 
{ 
   data_packing(shutDown,operatingMode); 
}

void decode_7219(int decodeMode) 
{ 
   data_packing(decode, decodeMode); 
}

void brightness_7219(int brightnessLevel) 
{ 
   data_packing(brightness, brightnessLevel); 
}


void scanLimit_7219(int totalDigitNumber) 
{ 
   data_packing(scanLimit, totalDigitNumber); 
} 

void test_7219(int testMode) 
{ 
   data_packing(dispTest, testMode); 
}

void init_7219() 
{ 
   shutdown_7219(1);     // No-Shutdown mode. Normal Operation mode. 
   decode_7219(0);      // All digits are programmed as code-B decode mode. 
   scanLimit_7219(7);    // Total digit number set to 4. 
   brightness_7219(15);  // Full brightness. 
}

void render_dotmatrix(int* display)
{
   int i;
   for(i = 0; i < 8; i++){
      write_data(i+1, *display);
      display++;
   }
}

