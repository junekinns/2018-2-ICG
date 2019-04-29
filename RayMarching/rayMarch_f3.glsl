#version 430 core
uniform float time;
uniform vec2 resolution;
uniform vec4 mouse;
uniform float s_time;
out vec4 fragColor;

#define PI 3.1415926535898 // Always handy.

const float eps = 0.005;
const int maxIterations = 128; 
const int maxIterationsRef = 32; 
const int maxIterationsShad = 32;
const float stepScale = 0.5; 
const float stepScaleRef = 0.95;
const float stopThreshold = 0.01; 

// Globals.
vec3 lp = vec3(0.);
vec3 surfNormal = vec3(0.);
vec3 ref = vec3(0.);
vec3 lookAt = vec3(0.);
vec3 camPos = vec3(0.);

// 2x2 matrix rotation.
mat2 rot2( float angle ){
	float c = cos( angle );
	float s = sin( angle );
	return mat2(
		 c, s,
		-s, c
	);
}

float planeXZ(in vec3 p, float dist) {
	return p.y - dist;
}

// Distance field equation for a sphere.
float sphere(in vec3 p, in vec3 centerPos, float radius){
	return length(p-centerPos) - radius;
}

// Sinusoidal plasma.
float sinusoidalBumps(in vec3 p){
    return sin(p.x*4.+time*0.97)*cos(p.y*4.+time*2.17)*sin(p.z*4.-time*1.31) + 0.5*sin(p.x*8.+time*0.57)*cos(p.y*8.+time*2.11)*sin(p.z*8.-time*1.23);
}

vec3 hashCheapAnimate(vec3 p){
    //p = sin(p); // A hack to ease degradation when using large input values.
    return cos(vec3( 174.3637*p.y*p.z + 13.53*p.x*(p.x-123.667),
                     219.3379*p.x*p.z + 11.79*p.y*(p.y-119.365),
                     223.3261*p.x*p.y + 13.87*p.z*(p.z-113.913)
                 ) + vec3(0.93,1.17, 1.01)*time)*0.5;
}

float cheapAnimateVoronoi(vec3 uv) {
	vec3 g = floor(uv);
	vec3 f = fract(uv);
	vec3 b = vec3(0.), o = b, r = b;

	float d = 0.36; // The highest possible maximum is sqrt(3), by the way.

	for(float j = -0.5; j < 1.51; j++) {
	    for(float i = -0.5; i < 1.51; i++) {
		    b = vec3(i, j, -0.5);
		    r = b - f + hashCheapAnimate(g+b);
		    d = min(d, dot(r,r));
		    b.z = 0.5;
		    r = b - f + hashCheapAnimate(g+b);
		    d = min(d, dot(r,r));
		    b.z = 1.5;
		    r = b - f + hashCheapAnimate(g+b);
		    d = min(d, dot(r,r));
	    }
	}
	return d/1.36; // Square roots, and so forth, are left to the discretion of the user.
}
// The bump mapping function.
float bumpFunction(in vec3 p){
     if(p.y>-0.8) return sqrt(cheapAnimateVoronoi(p*16.));
     else return sqrt(cheapAnimateVoronoi(p*4.));
}

// Bump mapping.
vec3 doBumpMap( in vec3 p, in vec3 nor, float bumpfactor ){
	float ref = bumpFunction( p );
    vec3 grad = vec3( bumpFunction(vec3(p.x+eps, p.y, p.z))-ref,
                      bumpFunction(vec3(p.x, p.y+eps, p.z))-ref,
                      bumpFunction(vec3(p.x, p.y, p.z+eps))-ref )/eps;
    grad -= nor*dot(grad, nor);
    return normalize( nor - bumpfactor*grad );
}

// This is the raymarched scene that gets called multiple times from the raymarching procedure, which means adding detail can slow things right down.
float scene(in vec3 p){
    float distortedObject = sphere(p, vec3(0., 1.1 , 2.5), 1.) + 0.3*sinusoidalBumps(p);
	float distortedFloor = planeXZ(p, -1.0) + 0.1*sinusoidalBumps(p*0.5);
	return min(distortedObject, distortedFloor);
}

vec3 getNormal(in vec3 p) {
	return normalize(vec3(
		scene(vec3(p.x+eps,p.y,p.z))-scene(vec3(p.x-eps,p.y,p.z)),
		scene(vec3(p.x,p.y+eps,p.z))-scene(vec3(p.x,p.y-eps,p.z)),
		scene(vec3(p.x,p.y,p.z+eps))-scene(vec3(p.x,p.y,p.z-eps))
	));
}

// Raymarching.
float rayMarching( vec3 origin, vec3 dir, float start, float end ) {
    float sceneDist = 1e4;
	float rayDepth = start; // Ray depth. "start" is usually zero, but for various reasons, you may wish to start the ray further away from the origin.
	for ( int i = 0; i < maxIterations; i++ ) {
		sceneDist = scene( origin + dir * rayDepth );
		if (( sceneDist < stopThreshold ) || (rayDepth >= end)) {
			break;
		}
		rayDepth += sceneDist * stepScale;
	}

	if ( sceneDist >= stopThreshold ) rayDepth = end;
	else rayDepth += sceneDist;
	return rayDepth;
}

float rayMarchingReflections( vec3 origin, vec3 dir, float start, float end ) {
	float sceneDist = 1e4;
	float rayDepth = start; // Ray depth. "start" is usually zero, but for various reasons, you may wish to start the ray further away from the origin.
	for ( int i = 0; i < maxIterationsRef; i++ ) {
		sceneDist = scene( origin + dir * rayDepth ); // Distance from the point along the ray to the nearest surface point in the scene.

		if (( sceneDist < stopThreshold ) || (rayDepth >= end)) {
			break;
		}
		rayDepth += sceneDist * stepScaleRef;
	}
	if ( sceneDist >= stopThreshold ) rayDepth = end;
	else rayDepth += sceneDist;
	return rayDepth;
}

vec3 lighting( vec3 sp, vec3 camPos, int reflectionPass){
    // Start with black.
    vec3 sceneColor = vec3(0.0);

    // Object's color.
    vec3 voxPos = mod(sp*0.5, 1.0);
    vec3 objColor = vec3(0.94, 0.85, 0.1);
	float bumps =  bumpFunction(sp);
	vec3 tinge = vec3(bumps);
	objColor = objColor*tinge;
    surfNormal = getNormal(sp);
    if (reflectionPass==0)surfNormal = doBumpMap(sp, surfNormal, 0.04); 

    // Lighting.

    vec3 ld = lp-sp;
    // lcolor - Light color.
    vec3 lcolor = vec3(1.,0.97,0.92);

     // Light falloff (attenuation).
    float len = length( ld ); // Distance from the light to the surface point.
    ld /= len; // Normalizing the light-to-surface, aka light-direction, vector.
    float lightAtten = min( 1.0 / ( 0.05*len*len ), 1.0 ); // Keeps things between 0 and 1.
	ref = reflect(-ld, surfNormal);

    float ambient = .1; //The object's ambient property.
    float specularPower = 19.0; // The power of the specularity. Higher numbers can give the object a harder, shinier look.
    float diffuse = max( 0.0, dot(surfNormal, ld) ); //The object's diffuse value.
    float specular = max( 0.0, dot( ref, normalize(camPos-sp)) ); //The object's specular value.
    specular = pow(specular, specularPower); 
    sceneColor += (objColor*(diffuse*0.9+ambient)+specular*0.5)*lcolor*lightAtten;
    return sceneColor;
}


void main(void) {
    vec2 aspect = vec2(resolution.x/resolution.y, 1.0); //
	vec2 screenCoords = (2.0*gl_FragCoord.xy/resolution.xy - 1.0)*aspect;
	// Camera Setup.
	// Camera movement.
	vec3 lookAt = vec3(0.,1.1,2.5);  // This is the point you look towards
	vec3 camPos = vec3(0.+3.25*cos(time*0.125), 2., 2.5+3.25*sin(time*0.125)); 

    lp = vec3(camPos.x+0.25, camPos.y+2.0, camPos.z+0.5);

    vec3 forward = normalize(lookAt-camPos);
    vec3 right = normalize(vec3(forward.z, 0., -forward.x )); 
    vec3 up = normalize(cross(forward,right));

    // FOV - Field of view.
    float FOV = 0.5;
    // ro - Ray origin.
    vec3 ro = camPos;
    // rd - Ray direction.
    vec3 rd = normalize(forward + FOV*screenCoords.x*right + FOV*screenCoords.y*up);
	// The screen's background color.
    vec3 bgcolor = vec3(0.);
	// Ray marching.
	const float clipNear = -1.0;
	const float clipFar = 16.0;
	float dist = rayMarching(ro, rd, clipNear, clipFar );
	if ( dist >= clipFar ) {
	    fragColor = vec4(bgcolor, 1.0);
	    return;
	}

	vec3 sp = ro + rd*dist;
	vec3 sceneColor = lighting( sp, camPos, 0);
	fragColor = vec4(clamp(sceneColor, 0.0, 1.0), 1.0);
}
