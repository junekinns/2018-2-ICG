#version 330 core

uniform float time;
uniform vec2 resolution;
uniform vec4 mouse;
out vec4 fragColor;
#define PI 3.1415926535898 // Always handy.

const float eps = 0.005;

// Gloable variables for the raymarching algorithms.
const int maxIterations = 128;
const int maxIterationsRef = 32;
const int maxIterationsShad = 24;
const float stepScale = 0.5;
const float stepScaleRef = 0.95;
const float stopThreshold = 0.005; 
vec3 lp = vec3(0.);
vec3 surfNormal = vec3(0.);
vec3 ref = vec3(0.);

// Distance field equation for a sphere.

// Distance field equation for a cube.
float box(vec3 p, vec3 b){
	vec3 d = abs(p) - b;
	return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float scene(in vec3 p) {
    p = mod(p, 1.0) - 0.5;//repeat
    float d = box(p, vec3(0.2,0.15,0.25)); 
	return d;
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
	float rayDepth = start; 
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

vec3 lighting( vec3 sp, vec3 camPos, int reflectionPass){
    // Start with black.
    vec3 sceneColor = vec3(0.0);
    vec3 voxPos = mod(sp*0.5, 1.0);
    vec3 objColor = vec3(sin(time*2), sin(time / 5), sin(time / 2))*0.975 + sin(voxPos*8.0)*0.05;
    if ((voxPos.x<0.5)&&(voxPos.y>=0.5)&&(voxPos.z>=0.5) ) objColor = 0.5+0.5*sin(voxPos*1.625*PI);
    else if ( (voxPos.x>=0.5)&&(voxPos.y<0.5)&&(voxPos.z<0.5) ) objColor = 0.5+0.5*sin(voxPos*1.625*PI);
    objColor = clamp(objColor, 0.0, 1.0);
    surfNormal = getNormal(sp);
    // Lighting.
    // lp - Light position. Keeping it in the vacinity of the camera, but away from the objects in the scene.
    lp = vec3(0.25*sin(time), 0.75, 0.25*cos(time)+time);
    // ld - Light direction.
    vec3 ld = lp-sp;
    // lcolor - Light color.
    vec3 lcolor = vec3(1.,0.98,0.95);

     // Light falloff (attenuation).
    float len = length( ld ); // Distance from the light to the surface point.
    ld /= len; // Normalizing the light-to-surface, aka light-direction, vector.
    float lightAtten = min( 1.0 / ( 0.25*len*len ), 1.0 ); // Keeps things between 0 and 1.

    // Obtain the reflected vector at the scene position "sp."
    ref = reflect(-ld, surfNormal);
    float ao = 1.0; 
    float ambient = .05; //The object's ambient property.
    float specularPower = 8.0; // The power of the specularity. Higher numbers can give the object a harder, shinier look.
    float diffuse = max( 0.0, dot(surfNormal, ld) ); //The object's diffuse value.
    float specular = max( 0.0, dot( ref, normalize(camPos-sp)) ); //The object's specular value.
    specular = pow(specular, specularPower); // Ramping up the specular value to the specular power for a bit of shininess.
    // Bringing all the lighting components togethr to color the screen pixel.
    sceneColor += (objColor*(diffuse*0.8+ambient)+specular*0.5)*lcolor*lightAtten*ao;
    return sceneColor;
}

void main(void) {
    vec2 aspect = vec2(resolution.x/resolution.y, 1.0); 
	vec2 screenCoords = (2.0*gl_FragCoord.xy/resolution.xy - 1.0)*aspect;

	vec3 lookAt = vec3(0., 1.*sin(time*0.5), time);  // This is the point you look towards.
	vec3 camPos = vec3(1.0*sin(time*0.5), 0.15*sin(time*0.25), 1.0*cos(time*0.5)+time); 

    vec3 forward = normalize(lookAt-camPos); // Forward vector.
    vec3 right = normalize(vec3(forward.z, 0., -forward.x )); 
    vec3 up = normalize(cross(forward,right)); // Cross product the two vectors above to get the up vector.

    // FOV - Field of view.
    float FOV = 0.5;

    // ro - Ray origin.
    vec3 ro = camPos;
    // rd - Ray direction.
    vec3 rd = normalize(forward + FOV*screenCoords.x*right + FOV*screenCoords.y*up);

	// The screen's background color.
    vec3 bgcolor = vec3(0.);

	// Ray marching.
	const float clipNear = 0.0;
	const float clipFar = 16.0;
	float dist = rayMarching(ro, rd, clipNear, clipFar );
	if ( dist >= clipFar ) {
	    fragColor = vec4(bgcolor, 1.0);
	    return;
	}

	vec3 sp = ro + rd*dist;
	vec3 sceneColor = lighting( sp, camPos, 0);

	if ( dist >= clipFar ) {
	    fragColor = vec4(clamp(sceneColor, 0.0, 1.0), 1.0);
	    return;
	}
    sceneColor += lighting( sp, sp, 1);
	fragColor = vec4(clamp(sceneColor, 0.0, 1.0), 1.0);
}
