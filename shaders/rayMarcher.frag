#version 410 core

// for debugging
float map_hit = 0.0;


in vec2 clipCoord;

out vec4 fragColor;

#define PRIMITIVE_NONE      0
#define PRIMITIVE_CUBE      1
#define PRIMITIVE_CONE      2
#define PRIMITIVE_CYLINDER  3
#define PRIMITIVE_TORUS     4
#define PRIMITIVE_SPHERE    5
#define PRIMITIVE_QUAD      6
#define PRIMITIVE_MESH      7

#define DISPLACEMENT_FACTOR 0.1

#define NO_TEX                0
#define SPHERE_SMOOTH_TEX     1
#define SPHERE_PATTERNED_TEX  2
#define SPHERE_PATTERNED_BUMP 3
#define QUAD_SMOOTH_TEX       4
#define QUAD_PATTERNED_TEX    5
#define QUAD_PATTERNED_BUMP   6


// camera information
uniform vec3  eye;
uniform vec3  look;
uniform vec3  up;
uniform float aspectRatio;

// Light data
const int MAX_LIGHTS = 10;

uniform int  lightTypes     [MAX_LIGHTS];   // 0 for point, 1 for directional
uniform vec3 lightPositions [MAX_LIGHTS];   // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightColors    [MAX_LIGHTS];
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term

// Textures
const int MAX_SAMPLERS = 6;
uniform sampler2D sphereSmoothTex;
uniform sampler2D spherePatternedTex;
uniform sampler2D spherePatternedBump;
uniform sampler2D quadSmoothTex;
uniform sampler2D quadPatternedTex;
uniform sampler2D quadPatternedBump;

// Uniform number limit: 4096
// Primitive data
const   int MAX_PRIMITIVES = 50;

uniform int  primitiveType          [MAX_PRIMITIVES];
uniform mat4 primitiveTransform     [MAX_PRIMITIVES];

uniform vec3 primitiveDiffuse       [MAX_PRIMITIVES];
uniform vec3 primitiveAmbient       [MAX_PRIMITIVES];
uniform vec3 primitiveReflective    [MAX_PRIMITIVES];
uniform vec3 primitiveSpecular      [MAX_PRIMITIVES];
uniform vec3 primitiveTransparent   [MAX_PRIMITIVES];
uniform vec3 primitiveEmissive      [MAX_PRIMITIVES];

uniform float primitiveShininess    [MAX_PRIMITIVES];
uniform float primitiveIOR          [MAX_PRIMITIVES]; // index of refraction

uniform float primitiveBlend [MAX_PRIMITIVES];

uniform int primitiveTexID  [MAX_PRIMITIVES];
uniform int primitiveBumpID [MAX_PRIMITIVES];


// Data structure for raymarching results
struct PrimitiveDist {
    float dist;
    int index;
};

vec3 texCube( sampler2D sam, in vec3 p, in vec3 n )
{
    return vec3( abs(n.x) * texture(sam, p.yz)
               + abs(n.y) * texture(sam, p.xz)
               + abs(n.z) * texture(sam, p.xy)
               );
}

// displacement resulted from bump map
float texDisplace = 0.0;

// Signed distance to the sphere.
float sdSphere(vec3 p) {
    float dist = length(p) - 0.5;
    return dist + texDisplace;
}

float dot2(vec3 v) { return dot(v,v); }
float udQuad(vec3 p) {
    vec3 a = vec3(-0.5, -0.5, 0.0);
    vec3 b = vec3(-0.5,  0.5, 0.0);
    vec3 c = vec3( 0.5,  0.5, 0.0);
    vec3 d = vec3( 0.5, -0.5, 0.0);

    vec3 ba = b - a; vec3 pa = p - a;
    vec3 cb = c - b; vec3 pb = p - b;
    vec3 dc = d - c; vec3 pc = p - c;
    vec3 ad = a - d; vec3 pd = p - d;
    vec3 nor = cross( ba, ad );

    float dist = sqrt(
    (sign(dot(cross(ba,nor),pa)) +
    sign(dot(cross(cb,nor),pb)) +
    sign(dot(cross(dc,nor),pc)) +
    sign(dot(cross(ad,nor),pd))<3.0)
    ?
    min( min( min(
    dot2(ba*clamp(dot(ba,pa)/dot2(ba),0.0,1.0)-pa),
    dot2(cb*clamp(dot(cb,pb)/dot2(cb),0.0,1.0)-pb) ),
    dot2(dc*clamp(dot(dc,pc)/dot2(dc),0.0,1.0)-pc) ),
    dot2(ad*clamp(dot(ad,pd)/dot2(ad),0.0,1.0)-pd) )
    :
    dot(nor,pa)*dot(nor,pa)/dot2(nor) );

    return dist + texDisplace;
}


PrimitiveDist map(vec3 p) {
    int   min_index = -1;
    float min_dist = 1.0 / 0.0; // requires GLSL 4.1+ to be infinite

//    for (int i = 0; i < MAX_PRIMITIVES; i++) {
    for (int i = 0; i < 10; i++) {
        mat4 transform = primitiveTransform[i];

        // assuming uniform scaling
        float scalingFactor = length(vec3(transform[0][0], transform[1][0], transform[2][0]));
//        float scalingFactor = 1.0;
//        mat3 rotationMatrix = (1.0 / scalingFactor) * mat3(transform);
//        vec3 translationVector = transform[3].xyz;
        vec3 obj_p = vec3(inverse(transform) * vec4(p, 1.0));

        float d = min_dist;
        switch (primitiveType[i]) {
        case PRIMITIVE_SPHERE:
            d = sdSphere(obj_p) * scalingFactor;
            break;

        case PRIMITIVE_QUAD:
            if (obj_p.z > 0) // do not show if faced backward
                d = udQuad(obj_p) * scalingFactor;
            break;

        default:
            /* NOT IMPLEMENTED */
            break;
        }

        if (d < min_dist) {
            min_index = i;
            min_dist = d;
        }
    }

    return PrimitiveDist(min_dist, min_index);
}

PrimitiveDist mapNoPlane(vec3 p) {
    int   min_index = -1;
    float min_dist = 1.0 / 0.0; // requires GLSL 4.1+ to be infinite

//    for (int i = 0; i < MAX_PRIMITIVES; i++) {
    for (int i = 0; i < 10; i++) {
        mat4 transform = primitiveTransform[i];
        float scalingFactor = length(vec3(transform[0][0], transform[1][0], transform[2][0]));
        vec3 obj_p = vec3(inverse(transform) * vec4(p, 1.0));

        float d = min_dist;
        switch (primitiveType[i]) {
        case PRIMITIVE_SPHERE:
            d = sdSphere(obj_p) * scalingFactor;
            break;

            /* PRIMITIVE_QUAD IGNORED */
        default:
            /* NOT IMPLEMENTED */
            break;
        }

        if (d < min_dist) {
            min_index = i;
            min_dist = d;
        }
    }

    return PrimitiveDist(min_dist, min_index);
}

const float epsilon = 0.001;
vec2 e = vec2(epsilon, 0.0); // For swizzling
vec3 calcNormal(vec3 p) {
    //return normalize(p);
    return normalize(vec3( map(p + e.xyy).dist - map(p - e.xyy).dist
                         , map(p + e.yxy).dist - map(p - e.yxy).dist
                         , map(p + e.yyx).dist - map(p - e.yyx).dist
                         ));
}

float shadow(vec3 ro, vec3 rd, float k) {
    float marchDist = 0.001;
    float boundingVolume = 25.0;
    float darkness = 1.0;
    float threshold = 0.001;

    for(int i = 0; i < 30; i++) {
        if(marchDist > boundingVolume) continue;
        float h = mapNoPlane(ro + rd * marchDist).dist;
        if (h < threshold) {
            darkness = 0.0;
            break;
        }
        darkness = min(darkness, k * h / marchDist);
        marchDist += h * 0.7;
    }

    return darkness;
}

PrimitiveDist raymarch(vec3 ro, vec3 rd) {
    float marchDist = 0.001;
    float boundingDist = 50.0;
    float threshold = 0.001;

    // Fill in the iteration count
    PrimitiveDist ret = PrimitiveDist(-1.0, -1);
    for (int i = 0; i < 1000; i++) {
        // Fill in loop body
        PrimitiveDist pd = map(ro + marchDist * rd);
        if (pd.dist < threshold) {
            ret.dist = marchDist + pd.dist;
            ret.index = pd.index;
            break;
        }

        marchDist += 0.2 * pd.dist;
        if (marchDist > boundingDist)
            break;
    }

    return ret;
}

vec3 getTex(int texID, vec3 pos, vec3 nor) {
    switch (texID) {
    case SPHERE_SMOOTH_TEX:
        return texCube(sphereSmoothTex, pos, nor);
    case SPHERE_PATTERNED_TEX:
        return texCube(spherePatternedTex, pos, nor);
    case SPHERE_PATTERNED_BUMP:
        return texCube(spherePatternedBump, pos, nor);
    case QUAD_SMOOTH_TEX:
        return texCube(quadSmoothTex, pos, nor);
    case QUAD_PATTERNED_TEX:
        return texCube(quadPatternedTex, pos, nor);
    case QUAD_PATTERNED_BUMP:
        return texCube(quadPatternedBump, pos, nor);
    default:
        return vec3(0.0);
    }
}

vec3 render(vec3 ro, vec3 rd, float t, int index) {
    vec3 pos = ro + rd * t;
    // Normal vector
    vec3 nor = calcNormal(pos);

    vec3 texture = getTex(primitiveTexID[index], pos, nor);
    float blend = primitiveBlend[index];

    texDisplace = getTex(primitiveBumpID[index], pos, nor).x;

    // recalculate pos and nor with bump applied
    pos = ro + rd * (t + texDisplace);
    nor = calcNormal(pos);

    // this useless line prevent GLSL optimization from overwriting primitiveShininess
    fragColor = vec4(primitiveIOR[index]);

    // Col is the final color of the current pixel.
    // Ambient
    vec3 col = primitiveAmbient[index];

    for (int i = 0; i < MAX_LIGHTS; i++) {
        // Light vector
        vec3 posToLight = vec3(0.);
        // Point Light
        if (lightTypes[i] == 0) {
            posToLight = normalize(lightPositions[i] - pos);
        } else if (lightTypes[i] == 1) {
            // Dir Light
            posToLight = normalize(-lightDirections[i]);
        }

        // Add diffuse component
        float diffuseIntensity = clamp(dot(nor, posToLight), 0.0, 1.0);
        vec3 diffuse = max(vec3(0), lightColors[i] * primitiveDiffuse[index] * diffuseIntensity);

        // Add specular component
        vec3 lightReflection = normalize(reflect(-posToLight, nor));
        vec3 eyeDirection = normalize(-rd);
        float shininess = primitiveShininess[index];
        float specIntensity = pow(clamp(dot(eyeDirection, lightReflection), 0.0, 1.0), shininess);
        vec3 specular = max(vec3(0), lightColors[i] * primitiveSpecular[index] * specIntensity);

        float darkness = shadow(pos, posToLight, 18.0);

        col += (diffuse + specular) * darkness;
    }

    return mix(col, texture, blend);
}

void main() {
    // Set up camera matrix
    vec3 cameraForward = normalize(look);
    vec3 cameraRight   = normalize(cross(cameraForward, up));
    vec3 cameraUp      = normalize(cross(cameraRight, cameraForward));

    vec2 uv = clipCoord;
    uv.x *= aspectRatio;

    // Construct world space ray
    float focalLength = 2; // IDK why but it is 2...
    vec3 rayDirection = vec3(uv, focalLength);
    rayDirection = rayDirection.x * cameraRight
                 + rayDirection.y * cameraUp
                 + rayDirection.z * cameraForward;
    rayDirection = normalize(rayDirection);

    // MARCH
    PrimitiveDist rayMarchResult = raymarch(eye, rayDirection);
    vec3 col = vec3(0);
    if (rayMarchResult.index >= 0) {
      	col = render(eye, rayDirection, rayMarchResult.dist, rayMarchResult.index);
    }

    fragColor = vec4(col, 1.0);
}
