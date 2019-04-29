#version 430
#define PI 3.1415926535898
uniform float time;
uniform vec2 resolution;
uniform vec4 mouse;
uniform float s_time;

out vec4 fragColor;
const float eps = 0.005;
const int maxIterations = 128;
const float stepScale = 0.5;
const float stopThreshold = 0.005; 

float sphere(in vec3 p, in vec3 centerPos, float radius) {
	return length(p-centerPos) - radius;
}

float scene(in vec3 p) {
	return sphere(p, vec3(0., 0. , 2.), 1.) + 0.04;
}

vec3 getNormal(in vec3 p) {
	// 6-tap normalization. Probably the most accurate, but a bit of a cycle waster.
	return normalize(vec3(
		scene(vec3(p.x+eps,p.y,p.z))-scene(vec3(p.x-eps,p.y,p.z)),
		scene(vec3(p.x,p.y+eps,p.z))-scene(vec3(p.x,p.y-eps,p.z)),
		scene(vec3(p.x,p.y,p.z+eps))-scene(vec3(p.x,p.y,p.z-eps))
	));
}

float rayMarching( vec3 origin, vec3 dir, float start, float end ) {
	float sceneDist = 1e4;
	float rayDepth = start; // Ray depth. "start" is usually zero, but for various reasons, you may wish to start the ray further away from the origin.
	for ( int i = 0; i < maxIterations; i++ ) {

		sceneDist = scene( origin + dir * rayDepth ); // Distance from the point along the ray to the nearest surface point in the scene.
	if (( sceneDist < stopThreshold ) || (rayDepth >= end)) {
		break;
	}
	rayDepth += sceneDist * stepScale;
	}
	if ( sceneDist >= stopThreshold ) rayDepth = end;
	else rayDepth += sceneDist;

	return rayDepth;
}

void main(void) {
    vec2 aspect = vec2(resolution.x/resolution.y, 1.0);
	vec2 screenCoords = (2.0*gl_FragCoord.xy/resolution.xy - 1.0)*aspect;
	vec3 lookAt = vec3(0.,0.,0.);  
	vec3 camPos = vec3(0., 0., -2.5); 
    // Camera setup.
    vec3 forward = normalize(lookAt-camPos); 
    vec3 right = normalize(vec3(forward.z, 0., -forward.x )); 
    vec3 up = normalize(cross(forward,right)); 
    float FOV = 0.25;

    vec3 ro = camPos;
    vec3 rd = normalize(forward + FOV*screenCoords.x*right + FOV*screenCoords.y*up);
    vec3 bgcolor = vec3(1.,0.97,0.92)*0.15;
    float bgshade = (1.0-length(vec2(screenCoords.x/aspect.x, screenCoords.y+0.5) )*0.8);
	bgcolor *= bgshade; //Shade the background a little.

	const float clipNear = 0.0;
	const float clipFar = 4.0;
	float dist = rayMarching(ro, rd, clipNear, clipFar ); // See the function itself for an explanation.
	if ( dist >= clipFar ) {
	    fragColor = vec4(bgcolor, 1.0);
	    return;
	}

	vec3 sp = ro + rd*dist;
	vec3 surfNormal = getNormal(sp);
	vec3 lp = vec3(1.5*sin(time*0.5), 0.75+0.25*cos(time*0.5), -1.0);
	vec3 ld = lp-sp;
	vec3 lcolor = vec3(1.,0.97,0.92);
	
	float len = length( ld ); // Distance from the light to the surface point.
	ld /= len; // Normalizing the light-to-surface, aka light-direction, vector.
	float lightAtten = min( 1.0 / ( 0.25*len*len ), 1.0 ); // Keeps things between 0 and 1.

	vec3 ref = reflect(-ld, surfNormal);

	vec3 sceneColor = vec3(0.0);
	vec3 objColor = vec3(1.5*sin(time*0.5), 0.75+0.25*cos(time*0.5), 1.0);
	float ambient = .1; 
	float specularPower = 16.0;
	float diffuse = max( 0.0, dot(surfNormal, ld));	
	float specular = max( 0.0, dot( ref, normalize(camPos-sp)) );
	specular = pow(specular, specularPower);
	sceneColor += (objColor*(diffuse*0.8+ambient)+specular*0.8)*lcolor*lightAtten;
	fragColor = vec4(sceneColor, 1.0);
}

