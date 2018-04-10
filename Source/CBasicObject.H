#ifndef _CBasicObject_H_
#define _CBasicObject_H_
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define OBJECT_TYPE_RESOURCE	1
#define OBJECT_TYPE_SPECIMEN	2
#define OBJECT_TYPE_OBSTACLE	3
#define OBJECT_TYPE_TRANSMITTER	4

#define ACCUMULATION_RATE	0.50f
#define GENERATION_COST		100.0f

#define MIN_VELOCITY        1.5f
#define MAX_VELOCITY        5.0f

#define OBSTACLE_HEIGHT		32
#define OBSTACLE_WIDTH		32		

#define SPECIMEN_HEIGHT		16
#define SPECIMEN_WIDTH		16

#define RESOURCE_HEIGHT		18
#define RESOURCE_WIDTH		18

#define TRANSMITTER_HEIGHT	100
#define TRANSMITTER_WIDTH	100

#define TRANSMIT_O_NOTHING	0
#define TRANSMIT_O_PRESENT	1

#define NUMBER_OF_ROUNDS	1000

#define OBJ_ATTRIB_TEMPORARY       0x00000001
#define OBJ_ATTRIB_ALLOW_INTERSECT 0x00000002 //Object is temporary (Bullets).
#define OBJ_ATTRIB_HASTRANSMITTER  0x00000004 //Object cann be intersected.
#define OBJ_ATTRIB_ISONRESOURCE    0x00000010 
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00000020  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00000040  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00000080  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00000100  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00000200  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00000400  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00000800  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00001000  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00002000  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00004000  
//#define OBJ_ATTRIB_XXXXXXXXXX    0x00010000  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBasicObject {
public:
	HBITMAP Texture;

	int Width;
	int Height;

	float VelocityX;
	float VelocityY;
	float Angle;

	float X;
	float Y;

	float ResourceCount;

	int Attributes;

	bool IsOptimistic;

	short ModelIndex;
	short ObjectType;

	int ArrayIndex;

	bool IsObjectInUse; //Is this object free? (when used in an array)
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

