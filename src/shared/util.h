#ifndef _Util_H
#define _Util_H

//$Id

#ifndef PI
#define PI	3.1415926535897932
#endif

#ifndef ARC
//#define ARC  PI/180.0
#define ARC	0.0174532925199433
#endif

#ifndef DEG
//#define DEG  180.0/PI
#define DEG	57.2957795130823
#endif

#define Cos(th) cos(ARC*(th))
#define Sin(th) sin(ARC*(th))

#endif

