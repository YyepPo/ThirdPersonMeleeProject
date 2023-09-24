#pragma once

#include "DrawDebugHelpers.h"


#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(),Location,25.f,12,FColor::Blue,true);
#define DRAW_SPHERE_SINGLEFRAME(Location) if(GetWorld()) DrawDebugSphere(GetWorld(),Location,25.f,12,FColor::Blue,false,-1.f);

#define DRAW_Line(StartLocation,EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,true,-1.f,0,1.f);
#define DRAW_Line_SINGLEFRAME(StartLocation,EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,false,-1.f,0,1.f);


#define	DRAW_POINT(EndLocation) if(GetWorld()) DrawDebugPoint(GetWorld(),EndLocation,15.f,FColor::Red,true);
#define	DRAW_POINT_SINGLEFRAME(EndLocation) if(GetWorld()) DrawDebugPoint(GetWorld(),EndLocation,15.f,FColor::Red,false,-1.f);