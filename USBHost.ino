// Simple test of USB Host
//
// This example is in the public domain

#include "USBHost_t36.h"
#include "usbd480.h"
#include "SdFat.h"


USBHost myusb;
USBHub hub1(myusb);
USBD480Display usbd480(myusb);
SdFatSdioEX sdEx;




#define TORUS_MAJOR     1.50f
#define TORUS_MINOR     0.50f
#define TORUS_MAJOR_RES	28		// rings
#define TORUS_MINOR_RES 28		// points
#define TORUS_1_RING	50

#define DEGTORAD 0.034906585039

#define RED		RGB_16_RED
#define GREEN	RGB_16_GREEN
#define BLUE	RGB_16_BLUE
#define WHITE	RGB_16_WHITE

#define BWIDTH			200
#define BHEIGHT			200


static int doTests = 0;



inline bool sdBusy ()
{
	return sdEx.card()->isBusy();
}

bool sdInit ()
{
	if (!sdEx.begin()){
      Serial.println("sdEx.begin() failed");
      return false;
	}

	// make sdEx the current volume.
	sdEx.chvol();
	return 1;
}


static inline void setPixel (uint8_t *buffer, const int x, const int y, uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;	
	
//	if (x >= BWIDTH  || x < 0) return;
//	if (y >= BHEIGHT || y < 0) return;
	
	pixels[(y*BWIDTH) + x] = colour;
}


static inline void xyzToPoint (const float x,const float y,const float z,const float flength,const float camz, const int x0, const  int y0, int *screenx, int *screeny)
{
	if (z-camz < 0.0f) return;
	float tmp = flength/(z-camz);
	*screenx = (x*tmp)+x0;
	*screeny = (y*tmp)+y0;
}

void torus2 (uint8_t *des, float xang, float yang, float zang, float flength, float zoom, int destx, int desty)
{
    int i, j, k=1;
    float s, t, x, y, z, nx, ny, nz, scale;
	float x2,y2,z2;
    int sx=0,sy=0,nsx=0,nsy=0;
	
	//pre calc some values
	float twopi = M_PI*2.0f;
	float ThetaX = xang * DEGTORAD;
	float ThetaY = yang * DEGTORAD;
	float ThetaZ = zang * DEGTORAD;
	float cosx = cosf(ThetaX);
	float cosy = cosf(ThetaY);
	float cosz = cosf(ThetaZ);
	float sinx = sinf(ThetaX);
	float siny = sinf(ThetaY);
	float sinz = sinf(ThetaZ);
	float twopiTORUS_MAJOR_RES = twopi/TORUS_MAJOR_RES;
	float twopiTORUS_MINOR_RES = twopi/TORUS_MINOR_RES;
	float sintres=0.0f;
	float costres=0.0f;
	float cosfres=0.0f;

        for( i = 0; i < TORUS_MINOR_RES; i++ ) {
            for( j = 0; j <= TORUS_MAJOR_RES; j++ ) {
        //        for( k = 1; k >= 0; k-- ) {
                    s = (i + k) % TORUS_MINOR_RES + 0.5f;
                    t = j % TORUS_MAJOR_RES;

                    // Calculate point on surface
                    // could put xyz into an array but thats no fun
                    sintres = sinf(t*twopiTORUS_MAJOR_RES);
                    costres = cosf(t*twopiTORUS_MAJOR_RES);
                    cosfres = cosf(s*twopiTORUS_MINOR_RES);
                    
                    x = (TORUS_MAJOR+TORUS_MINOR*cosfres)*costres;
                    y = TORUS_MINOR * sinf(s*twopiTORUS_MINOR_RES);
                    z = (TORUS_MAJOR+TORUS_MINOR*cosfres)*sintres;

                    // Calculate surface normal
                    nx = x - TORUS_MAJOR*costres;
                    ny = y;
                    nz = z - TORUS_MAJOR*sintres;
                    scale = 1.0f / sqrtf( nx*nx + ny*ny + nz*nz);
                    nx *= scale;
                    ny *= scale;
                    nz *= scale;

  					//RotateX(ThetaX, &ny, &nz);
  	  				y2 = (ny*cosx) - (nz*sinx);
					nz = (nz*cosx) + (ny*sinx);
					ny = y2;
									
  					//RotateY(ThetaY, &nx, &nz);
  					z2 = (nz*cosy) - (nx*siny);
					nx = (nx*cosy) + (nz*siny);
					nz = z2;
					  					
   					//RotateZ(ThetaZ, &nx, &ny);
					x2 = (nx*cosz) - (ny*sinz);
					ny = (ny*cosz) + (nx*sinz);
					nx = x2;


  					//RotateX(ThetaX, &y, &z);
  					y2 = (y*cosx) - (z*sinx);
					z = (z*cosx) + (y*sinx);
					y = y2;
				
  					//RotateY(ThetaY, &x, &z);
  					z2 = (z*cosy) - (x*siny);
					x = (x*cosy) + (z*siny);
					z = z2;
				
   					//RotateZ(ThetaZ, &x, &y);
					x2 = (x*cosz) - (y*sinz);
					y = (y*cosz) + (x*sinz);
					x = x2;
				
   					xyzToPoint(nx, ny, nz, flength, zoom, destx, desty, &nsx, &nsy);
          			setPixel(des, nsx, nsy, RED);
          			
          			//if (nz < 0.0){
            			xyzToPoint(x, y, z, flength, zoom, destx, desty, &sx, &sy);
            			setPixel(des, sx, sy, GREEN);
            		//}
             //   }
            }
        }
	return;
}

void torus1 (uint8_t *des, int numc, int numt, float xang, float yang, float zang, float flength, float zoom, int destx, int desty)
{
	int i, j, k;
	float s, t, x, y, z;
	float x2,y2,z2;
	int sx=0,sy=0;

	float twopi = 2.0f * M_PI;
	float ThetaX = xang * DEGTORAD;
	float ThetaY = yang * DEGTORAD;
	float ThetaZ = zang * DEGTORAD;
	float cosx = cosf(ThetaX);
	float cosy = cosf(ThetaY);
	float cosz = cosf(ThetaZ);
	float sinx = sinf(ThetaX);
	float siny = sinf(ThetaY);
	float sinz = sinf(ThetaZ);
	float cosftwonumc=0.0f;
	float twonumc=0.0f,twonumt=0.0f;
   
	for (i = 0; i < numc; i++) {
      for (j = 0; j <= numt; j++) {
         for (k = 1; k >= 0; k--) {
            s = (i + k) % numc + 0.5f;
            t = j % numt;


            x = (1.0f+0.1f*cosf(s*twopi/numc))*cosf(t*twopi/numt);
            y = (1.0f+0.1f*cosf(s*twopi/numc))*sinf(t*twopi/numt);
            z = 0.1f * sinf(s * twopi / numc);


			twonumc = s*twopi/numc;
			twonumt = t*twopi/numt;
			cosftwonumc = cosf(twonumc);
            x = (1.0f+0.1f*cosftwonumc)*cosf(twonumt);
            y = (1.0f+0.1f*cosftwonumc)*sinf(twonumt);
            z = 0.1f * sinf(twonumc);

			y2 = (y*cosx) - (z*sinx);
			z = (z*cosx) + (y*sinx);
			y = y2;
				
  			//RotateY(ThetaY, &x, &z);
  			z2 = (z*cosy) - (x*siny);
			x = (x*cosy) + (z*siny);
			z = z2;
				
   			//RotateZ(ThetaZ, &x, &y);
			x2 = (x*cosz) - (y*sinz);
			y = (y*cosz) + (x*sinz);
			x = x2;
   			
            xyzToPoint(x, y, z, flength, zoom, destx, desty, &sx, &sy);
            setPixel(des, sx, sy, BLUE);
         }
      }
   }
}

int gfxTest3 (uint8_t *buffer)
{
	const int destx = BWIDTH/2;
	const int desty = BHEIGHT/2;

	float step = 0.07f;
 	int ct = 0;
 
   	for (float a = 0.0f; a < 360.0f+step; a += step){
   		
   		// clear frame
   		memset(buffer, 0, BWIDTH * BHEIGHT * 2);
   		
   		torus1(buffer, 12, TORUS_1_RING, a*2.0f, a/2.0f, a, 350, -9.0f, destx, desty);
   		torus2(buffer,                   a/2.0f, a*2.0f, a, 350, -9.0f, destx, desty);

		usbd480.drawScreenArea(buffer, BWIDTH, BHEIGHT, 140, 36);
		ct++;
	}
	return ct;
}

void gfxTest2 (uint8_t *buffer)
{
    for (int y = 0; y < BHEIGHT; y++){
    	for (int x = 0; x < BWIDTH; x++){
			int r = (x>>3)&0x1F;
			int g = (y>>2)&0x3F;
			int b = 8&0x1F;
			setPixel(buffer, x, y, (r<<11)|(g<<5)|b);	// 16bit
	    }
	}
}

int gfxTest4 (uint8_t *buffer)
{
	const uint16_t colour[4] = {RED, GREEN, BLUE, WHITE};
	
	int ct;
	for (ct = 0; ct < 500; ct++)
		usbd480.fillScreen(colour[ct&0x03]);
		
	return ct;
}

#if 0
int sdToDisplayTest2 (const char *filename)
{
	File file;
	
	if (!file.open(filename, O_RDONLY)){
		Serial.println("file.open() failed");
		return 0;
	}
	
	int frame = 0;
	int bwidth = 480;
	int bheight = 272;
	size_t bufferLength = bwidth * 2;
	uint8_t buffer[(2*bufferLength)];

	
	usbd480.setLineLength(bwidth);
	delayMicroseconds(41);
	
	const uint32_t frameLength = (bwidth * bheight) - 1;
	uint32_t writeaddress = 0;
		
	buffer[0] = 0x41; // pixel write
	buffer[1] = 0x5B;
	buffer[2] =  writeaddress&0xFF;
	buffer[3] = (writeaddress>>8)&0xFF;
	buffer[4] = (writeaddress>>16)&0xFF;
	buffer[5] = (writeaddress>>24)&0xFF;
	buffer[6] =  frameLength&0xFF;
	buffer[7] = (frameLength>>8)&0xFF;
	buffer[8] = (frameLength>>16)&0xFF;
	buffer[9] = (frameLength>>24)&0xFF;


	usbd480.writeData(buffer, 10);
	delayMicroseconds(41);
	
	int ret = 0;
	
	for (frame = 0; frame < 1000; frame++){
		for (int y = 0; y < bheight; y+=2){
			/*size_t bytesRead =*/ file.read(buffer, bufferLength*2);			// load top half of frame
			delayMicroseconds(40);
			//if (bytesRead == bufferLength){
				ret += usbd480.writeData(buffer, bufferLength*2);
				delayMicroseconds(40);
			//}
		}

		//myusb.Task();
		delay(1);
	}
		
	file.close();
	return ret;//frame;
}
#endif

int sdToDisplayTest1 (const char *filename)
{
	File file;
	
	if (!file.open(filename, O_RDONLY)){
		Serial.println("file.open() failed");
		return 0;
	}
	
	int bwidth = 480;
	int bheight = 136;
	size_t bufferLength = bwidth * bheight * 2;
	uint8_t buffer[bufferLength];
	int frame = 0;

	uint32_t frameAddr[2] = {0, (uint32_t)(usbd480.display.width * usbd480.display.height)};
	//uint32_t frameAddr[2] = {(uint32_t)(usbd480.display.width * usbd480.display.height), (uint32_t)(usbd480.display.width * usbd480.display.height)};
	//const uint32_t frameAddr[2] = {0, 480*272};
	
	for (frame = 0; frame < 1000; frame++){
		uint32_t bufferIdx = frame&0x01;

		size_t bytesRead = file.read(buffer, bufferLength);				// load top half of frame
		if (bytesRead != bufferLength){
			Serial.println("file.read() length mismatch");
			Serial.printf("file.read() bufferLength %i\r\n", (int)bufferLength);
			Serial.printf("file.read() bytesRead %i\r\n", (int)bytesRead);
		}
		usbd480.drawScreenArea(buffer, bwidth, bheight, 0, 0, frameAddr[bufferIdx], 0);			// send to display

		bytesRead = file.read(buffer, bufferLength);					// load bottom half
		if (bytesRead != bufferLength){
			Serial.println("file.read() length mismatch");
			Serial.printf("file.read() bufferLength %i\r\n", (int)bufferLength);
			Serial.printf("file.read() bytesRead %i\r\n", (int)bytesRead);
		}
		usbd480.drawScreenArea(buffer, bwidth, bheight, 0, 136, frameAddr[bufferIdx], 1);		// send to display

		//myusb.Task();
		//delay(6);
	}
		
	file.close();
	return frame;
}

int sdToDisplayTest (const char *filename)
{
	uint32_t t2 = millis();
	int ct = sdToDisplayTest1(filename);
	uint32_t t3 = millis();
	
	float t = (float)(t3-t2) / (float)ct;
	Serial.printf("sdToDisplayTest:	 %s", filename);
	Serial.println();
	Serial.printf("  time: m:%i fps:%f", (int)t3-t2, 1.0f/(t/1000.0f));
	Serial.println();
	
	delay(200);
	return ct;
}

void gfxTest ()
{
	uint8_t buffer[BWIDTH*BHEIGHT*2];
	//memset(buffer, 255, sizeof(buffer));

	//usbd480.drawScreenArea(buffer, BWIDTH, BHEIGHT, 0, 0);

	gfxTest2(buffer);
	usbd480.drawScreenArea(buffer, BWIDTH, BHEIGHT, 480-1-BWIDTH, 272-1-BHEIGHT);
	
	//uint32_t t2 = micros();
	uint32_t t2 = millis();
	int ct = gfxTest3(buffer);
	uint32_t t3 = millis();
	//uint32_t t3 = micros();
	float t = (float)(t3-t2) / (float)ct;
	//Serial.printf("time: u:%i fps:%f", (int)t3-t2, 1.0f/(t/1000000.0f));
	Serial.printf("gfxTest3:	time: m:%i fps:%f", (int)t3-t2, 1.0f/(t/1000.0f));
	Serial.println();


	t2 = millis();
	ct = gfxTest4(buffer);
	t3 = millis();
	t = (float)(t3-t2) / (float)ct;
	
	//Serial.printf("time: u:%i fps:%f", (int)t3-t2, 1.0f/(t/1000000.0f));
	Serial.printf("gfxTest4:	time: m:%i fps:%f", (int)t3-t2, 1.0f/(t/1000.0f));
	Serial.println();
	
	gfxTest2(buffer);
	usbd480.drawScreenArea(buffer, BWIDTH, BHEIGHT, (480-BWIDTH)/2, (272-BHEIGHT)/2);
}

int driver_callback (uint32_t msg, intptr_t *value1, uint32_t value2)
{
	Serial.printf("driver_callback %i %i %i", (int)msg, value1, value2);
	Serial.println();
	
	if (msg == USBD_MSG_DISPLAYREADY){
		//delay(100);
		
		char name[DEVICE_NAMELENGTH+1];
		char serial[DEVICE_SERIALLENGTH+1];
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t version = 0;
	
		usbd480.getDeviceDetails(name, &width, &height, &version, serial);
		Serial.println(name);
		Serial.println(serial);
		Serial.printf("%i %i %i\r\n", width, height, version);

		delay(5);

		doTests = 1;
	}
	
	return 1;
}


void setup ()
{
	while (!Serial) ; // wait for Arduino Serial Monitor
	
	Serial.println("USB Host Testing");
	
	myusb.begin();
	usbd480.setCallbackFunc(driver_callback);
	delay(25);
	sdInit();
}

void loop ()
{
	myusb.Task();
	
	if (doTests){
		doTests = 0;

		sdToDisplayTest("Pastelpower-JeanPaul.raw");
		sdToDisplayTest("Pastelpower-JeanPaul.raw");
		
	}
}

