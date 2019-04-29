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
vec3 sdTwist( vec3 p, float a );

float sphere(in vec3 p, in vec3 centerPos, float radius) {
	return length(p-centerPos) - radius;
}

float sphereSDF_meta(vec3 loc, vec3 p){
   vec3 leng = loc - p;
   return length(leng) ;
}

float sinusoidBumps(in vec3 p){
    return sin(p.x*16.+time*0.57)*cos(p.y*16.+time*2.17)*sin(p.z*16.-time*1.31) + 0.5*sin(p.x*32.+time*0.07)*cos(p.y*32.+time*2.11)*sin(p.z*32.-time*1.23);
}

float scene(in vec3 p) {
	return sphere(p, vec3(0., 0. , 2.), 1.) + 0.04*sinusoidBumps(p);
	//return sphere(p, vec3(0., 0. , 2.), 1.) + 0.04;
}

float sceneSDF_meta(vec3 samplePoint) {
   float minDist = 0.280;
   vec3 l1 = vec3(sin(time*3.5)*2.3, sin(time+3.14), sin(time)*.1);
   vec3 l2 = vec3(sin(time*1.5+3.14)*2.2, sin(time), cos(time)*1.1);
   vec3 l3 = vec3(sin(time*2.3+10.45)*1.6,0.,cos(time*5.1)*.4);
   vec3 l4 = vec3(0., 0., 0.);
   float d1 = sphereSDF_meta(l1, samplePoint);
   float d2 = sphereSDF_meta(l2, samplePoint);
   float d3 = sphereSDF_meta(l3, samplePoint);
   float d4 = sphereSDF_meta(l4, samplePoint);
   float m = 2.0;
   float r = 0.514;
   m-=r/d1;
   m-=r/d2;
   m-=r/d3;
   m-=r/d4;
   return m-0.476;
}

vec3 getNormal(in vec3 p) {
	// 6-tap normalization. Probably the most accurate, but a bit of a cycle waster.
	return normalize(vec3(
		sceneSDF_meta(vec3(p.x+eps,p.y,p.z))-sceneSDF_meta(vec3(p.x-eps,p.y,p.z)),
		sceneSDF_meta(vec3(p.x,p.y+eps,p.z))-sceneSDF_meta(vec3(p.x,p.y-eps,p.z)),
		sceneSDF_meta(vec3(p.x,p.y,p.z+eps))-sceneSDF_meta(vec3(p.x,p.y,p.z-eps))
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

vec3 sdTwist( vec3 p, float a ) {
   float c = cos(a*p.y);
   float s = sin(a*p.y);
   mat2  m = mat2(c,-s,s,c);
   return vec3(m*p.xz,p.y);
}

const float num = 5.;
vec2 random(vec2 c_) {
	float x = fract(sin(dot(c_, vec2(75.8, 48.6)))*1e5);
	float y = fract(sin(dot(c_, vec2(85.8, 108.6)))*1e5);

	vec2 returnVec = vec2(x, y);
	returnVec = returnVec * 2. - 1.; 
	return returnVec;
}

float noise(vec2 coord) {
	vec2 i = floor(coord);
	vec2 f = fract(coord);
	f = smoothstep(0., 1., f); 
	float returnVal = mix(mix(dot(random(i), coord - i),
		dot(random(i + vec2(1., 0.)), coord - (i + vec2(1., 0.))),
		f.x),
		mix(dot(random(i + vec2(0., 1.)), coord - (i + vec2(0., 1.))),
			dot(random(i + vec2(1., 1.)), coord - (i + vec2(1., 1.))),
			f.x),
		f.y
	);
	return (returnVal);
}

float random_f(vec2 coord) {
	return fract(sin(dot(coord, vec2(75.7, 65.9)))*1e5); // 0~1
}

vec2 noiseVec2(vec2 coord) {
	float time_Speed = 0.05;
	float time_Diff = random_f(coord)*10.375;
	coord += time * time_Speed + time_Diff;
	return vec2(noise((coord + 0.57)), noise((coord + 90.43)));
}

vec2 randomSet(float c) {
	float x = fract(sin(c + 0.7)*1e5);
	float y = fract(sin(x + 2.352)*1e5);

	vec2 returnVec = vec2(x, y);
	returnVec = returnVec * 2. - 1.; //벡터값을 0 ~ 1을 -1 ~ 1로 맞춰놓는 작업이다 

	return returnVec;
}


void main(void) {
    vec2 aspect = vec2(resolution.x/resolution.y, 1.0);
	vec2 screenCoords = (2.0*gl_FragCoord.xy/resolution.xy - 1.0)*aspect;
	vec3 lookAt = vec3(0.,0.,0.);  
	vec3 camPos = vec3(0., 0., -1.); 
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
	
	float minDist = 1.;
	for (float i = 0.; i < num; i++) {
		vec2 temp = randomSet(i)*.1;
		temp += noiseVec2(temp)*5.;

		float dist = distance(screenCoords, temp);

		// minDist = min(minDist, dist);
		minDist = min(minDist, 2.612*minDist*dist);
	}
	//color = vec3(smoothstep(0.146, 0.334,minDist));
	// color = mix(vec3(0.000), vec3(1.00), sin(color*180.));
	//color = vec3(step(.1, minDist));

	vec3 sceneColor = vec3(0.0);
	vec3 objColor = vec3(1.0, 0.6, 0.8);
	float bumps =  sinusoidBumps(sp);
    objColor = clamp(objColor*0.8-vec3(0.4, 0.2, 0.1)*bumps, 0.0, 1.0);

	float ambient = .1; 
	float specularPower = 16.0;
	float diffuse = max( 0.0, dot(surfNormal, ld));
	
	float specular = max( 0.0, dot( ref, normalize(camPos-sp)) );
	specular = pow(specular, specularPower);
	//sceneColor += vec3(step(.5, minDist))*(objColor*(diffuse*0.8+ambient)+specular*0.5)*lcolor*lightAtten;
	sceneColor += (objColor*(diffuse*0.8+ambient)+specular*0.5)*lcolor*lightAtten;
	sceneColor += vec3(step(.5, minDist));
	fragColor = vec4(clamp(sceneColor, 0.0, 1.0), 1.0);
}

