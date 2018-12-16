#version 410 core

// for debugging
vec3 dbg = vec3(0.0);
#define FLOAT_INF (1.0 / 0.0) // requires GLSL 4.1+ to be infinite

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

#define INDEX_FACES -1

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
const int MAX_SAMPLERS = 5;
uniform sampler2D sphereSmoothTex;
uniform sampler2D spherePatternedTex;
uniform sampler2D spherePatternedBump;
uniform sampler2D quadTex;
uniform sampler2D quadBump;

// Uniform number limit: 4096
// Primitive data
const   int MAX_PRIMITIVES = 20;

// sphere only this time
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
uniform float primitiveDFact [MAX_PRIMITIVES]; // Displacement Factor

uniform int primitiveTexID  [MAX_PRIMITIVES];
uniform int primitiveBumpID [MAX_PRIMITIVES];

// face data
uniform mat4 faceTransform;

uniform vec3 faceDiffuse;
uniform vec3 faceAmbient;
uniform vec3 faceReflective;
uniform vec3 faceSpecular;
uniform vec3 faceTransparent;
uniform vec3 faceEmissive;

uniform float faceShininess;
uniform float faceIOR; // index of refraction

uniform float faceBlend;
uniform float faceDFact; // Displacement Factor

// Data structure for raymarching results
struct PrimitiveDist {
    float dist;
    int index;
};

// FIXME: use ony one texture for the time being
vec3 texCube( sampler2D sam, in vec3 p, in vec3 n )
{
    return vec3( abs(n.x) * texture(spherePatternedTex, p.yz)
               + abs(n.y) * texture(spherePatternedTex, p.xz)
               + abs(n.z) * texture(spherePatternedTex, p.xy)
               );
}

vec3 getTex(int texID, vec3 pos, vec3 nor) {
    switch (texID) {
    case SPHERE_SMOOTH_TEX:
        return texCube(sphereSmoothTex, pos, nor);
    case SPHERE_PATTERNED_TEX:
        return texCube(spherePatternedTex, pos, nor);
    case SPHERE_PATTERNED_BUMP:
        return texCube(spherePatternedBump, pos, nor);
    default:
        return vec3(0.0);
    }
}

// Signed distance to the sphere.
float sdSphere(vec3 p, mat4 transform, float texDisplace) {
    float scalingFactor = length(vec3(transform[0][0], transform[1][0], transform[2][0]));
//    mat3 rotationMatrix = (1.0 / scalingFactor) * mat3(transform);
    vec3 translationVector = transform[3].xyz;

    float radius = scalingFactor / 2;
    float dist = length(p - translationVector) - radius;
    return dist + texDisplace;
}

float dot2(vec3 v) { return dot(v,v); }
float udQuad(vec3 p, vec3 a, vec3 b, vec3 c, vec3 d, float texDisplace) {
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

PrimitiveDist boxMap(vec3 p, vec3 rd) {
    int   min_index = INDEX_FACES;
    float min_dist = FLOAT_INF;

    float dist;
    vec3 a, b, c, d;

    //+z
    vec3 nor = vec3(0, 0, -1);
    if (dot(nor, rd) < 0) {
        a = vec3(-1.0, -1.0, 1.0);
        b = vec3(-1.0,  1.0, 1.0);
        c = vec3( 1.0,  1.0, 1.0);
        d = vec3( 1.0, -1.0, 1.0);

        float texDisplace = (texCube(quadBump, p, nor).x - 1.0) * faceDFact;
        dist = udQuad(p, a, b, c, d, texDisplace);

        if (dist < min_dist) {
            min_dist = dist;
        }
    }

    //-z
    nor = vec3(0, 0, 1);
    if (dot(nor, rd) < 0) {
        a = vec3(-1.0, -1.0, -1.0);
        b = vec3(-1.0,  1.0, -1.0);
        c = vec3( 1.0,  1.0, -1.0);
        d = vec3( 1.0, -1.0, -1.0);

        float texDisplace = (texCube(quadBump, p, nor).x - 1.0) * faceDFact;
        dist = udQuad(p, a, b, c, d, texDisplace);

        if (dist < min_dist) {
            min_dist = dist;
        }
    }

    //+y
    nor = vec3(0, -1, 0);
    if (dot(nor, rd) < 0) {
        a = vec3(-1.0, 1.0, -1.0);
        b = vec3(-1.0, 1.0,  1.0);
        c = vec3( 1.0, 1.0,  1.0);
        d = vec3( 1.0, 1.0, -1.0);

        float texDisplace = (texCube(quadBump, p, nor).x - 1.0) * faceDFact;
        dist = udQuad(p, a, b, c, d, texDisplace);

        if (dist < min_dist) {
            min_dist = dist;
        }
    }

    //-y
    nor = vec3(0, 1, 0);
    if (dot(nor, rd) < 0) {
        a = vec3(-1.0, -1.0, -1.0);
        b = vec3(-1.0, -1.0,  1.0);
        c = vec3( 1.0, -1.0,  1.0);
        d = vec3( 1.0, -1.0, -1.0);

        float texDisplace = (texCube(quadBump, p, nor).x - 1.0) * faceDFact;
        dist = udQuad(p, a, b, c, d, texDisplace);

        if (dist < min_dist) {
            min_dist = dist;
        }
    }

    //+x
    nor = vec3(-1, 0, 0);
    if (dot(nor, rd) < 0) {
        a = vec3(1.0, -1.0, -1.0);
        b = vec3(1.0, -1.0,  1.0);
        c = vec3(1.0,  1.0,  1.0);
        d = vec3(1.0,  1.0, -1.0);

        float texDisplace = (texCube(quadBump, p, nor).x - 1.0) * faceDFact;
        dist = udQuad(p, a, b, c, d, texDisplace);

        if (dist < min_dist) {
            min_dist = dist;
        }
    }

    //-x
    nor = vec3(1, 0, 0);
    if (dot(nor, rd) < 0) {
        a = vec3(-1.0, -1.0, -1.0);
        b = vec3(-1.0, -1.0,  1.0);
        c = vec3(-1.0,  1.0,  1.0);
        d = vec3(-1.0,  1.0, -1.0);

        float texDisplace = (texCube(quadBump, p, nor).x - 1.0) * faceDFact;
        dist = udQuad(p, a, b, c, d, texDisplace);

        if (dist < min_dist) {
            min_dist = dist;
        }
    }

    return PrimitiveDist(min_dist, min_index);
}

PrimitiveDist sphereMap(vec3 p) {
    int   min_index;
    float min_dist = FLOAT_INF;

    for (int i = 0; i < MAX_PRIMITIVES; i++) {
        if (primitiveType[i] == PRIMITIVE_NONE)
            continue;

        mat4 transform = primitiveTransform[i];

        // assuming uniform scaling
//        float scalingFactor = length(vec3(transform[0][0], transform[1][0], transform[2][0]));
//        float scalingFactor = 1.0;
//        mat3 rotationMatrix = (1.0 / scalingFactor) * mat3(transform);
//        vec3 translationVector = transform[3].xyz;

        vec3 translationVector = transform[3].xyz;
        vec3 nor = normalize(p - translationVector);
        float texDisplace = (getTex(primitiveBumpID[i], p, nor).x - 1.0) * primitiveDFact[i];
        float d = sdSphere(p, transform, texDisplace);

        if (d < min_dist) {
            min_index = i;
            min_dist = d;
        }
    }

    return PrimitiveDist(min_dist, min_index);
}

const float epsilon = 0.001;
vec2 e = vec2(epsilon, 0.0); // For swizzling
vec3 calcBoxNormal(vec3 p, vec3 rd) {
    //return normalize(p);
    return normalize(vec3( boxMap(p + e.xyy, rd).dist - boxMap(p - e.xyy, rd).dist
                         , boxMap(p + e.yxy, rd).dist - boxMap(p - e.yxy, rd).dist
                         , boxMap(p + e.yyx, rd).dist - boxMap(p - e.yyx, rd).dist
                         ));
}
vec3 calcSphereNormal(vec3 p) {
    //return normalize(p);
    return normalize(vec3( sphereMap(p + e.xyy).dist - sphereMap(p - e.xyy).dist
                         , sphereMap(p + e.yxy).dist - sphereMap(p - e.yxy).dist
                         , sphereMap(p + e.yyx).dist - sphereMap(p - e.yyx).dist
                         ));
}


float shadow(vec3 ro, vec3 rd, float k) {
    float marchDist = 0.001;
    float boundingVolume = 25.0;
    float darkness = 1.0;
    float threshold = 0.001;

    for(int i = 0; i < 30; i++) {
        if (marchDist > boundingVolume) continue;
        float h = sphereMap(ro + rd * marchDist).dist;
        if (h < threshold) {
            if (marchDist < threshold) break;
            darkness = 0.0;
            break;
        }
        darkness = min(darkness, k * h / marchDist);
        marchDist += h * 0.8;
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
        vec3 p = ro + marchDist * rd;
        PrimitiveDist sd = sphereMap(p);
        PrimitiveDist pd = boxMap(p, rd);

        if (sd.dist < pd.dist)
            pd = sd;

        if (pd.dist < threshold) {
            ret.dist = marchDist;
            ret.index = pd.index;
            break;
        }

        marchDist += 0.8 * pd.dist;
        if (marchDist > boundingDist)
            break;
    }

    return ret;
}

vec3 renderBox(vec3 rd, vec3 pos, vec3 nor) {
    vec3 texture = texCube(quadTex, pos, nor);

    float blend = faceBlend;

    // Col is the final color of the current pixel.
    // Ambient
    vec3 col = faceAmbient;

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
        vec3 diffuseColor = mix(faceDiffuse, texture, blend);
        vec3 diffuse = max(vec3(0), lightColors[i] * diffuseColor * diffuseIntensity);

        // Add specular component
        vec3 lightReflection = normalize(reflect(-posToLight, nor));
        vec3 eyeDirection = normalize(-rd);
        float shininess = faceShininess;
        float specIntensity = pow(clamp(dot(eyeDirection, lightReflection), 0.0, 1.0), shininess);
        vec3 specular = max(vec3(0), lightColors[i] * faceSpecular * specIntensity);

        float darkness = shadow(pos + nor * 0.001, posToLight, 18.0);

        col += (diffuse + specular) * darkness;
    }

    return col;
}

vec3 render(vec3 rd, vec3 pos, vec3 nor, int index) {
    vec3 texture = getTex(primitiveTexID[index], pos, nor);

    float blend = primitiveBlend[index];

    // Col is the final color of the current pixel.
    // Ambient
    vec3 col = primitiveAmbient[index];

    for (int i = 0; i < MAX_LIGHTS; i++) {
        // Light vector
        vec3 posToLight = vec3(0.0);
        // Point Light
        if (lightTypes[i] == 0) {
            posToLight = normalize(lightPositions[i] - pos);
        } else if (lightTypes[i] == 1) {
            // Dir Light
            posToLight = normalize(-lightDirections[i]);
        }

        // Add diffuse component
        float diffuseIntensity = clamp(dot(nor, posToLight), 0.0, 1.0);
        vec3 diffuseColor = mix(primitiveDiffuse[index], texture, blend);
        vec3 diffuse = max(vec3(0), lightColors[i] * diffuseColor * diffuseIntensity);

        // Add specular component
        vec3 lightReflection = normalize(reflect(-posToLight, nor));
        vec3 eyeDirection = normalize(-rd);
        float shininess = primitiveShininess[index];
        float specIntensity = pow(clamp(dot(eyeDirection, lightReflection), 0.0, 1.0), shininess);
        vec3 specular = max(vec3(0), lightColors[i] * primitiveSpecular[index] * specIntensity);

        float darkness = shadow(pos + nor * 0.001, posToLight, 18.0);

        col += (diffuse + specular) * darkness;
    }

    return col;
}

vec3 findColor(vec3 ro, vec3 rd) {
    vec3 finalColor = vec3(0.0);
    vec3 frac = vec3(1.0);

    for (int raybounce = 0; raybounce < 5; raybounce++) {
        // MARCH
        PrimitiveDist rayMarchResult = raymarch(ro, rd);
        if (rayMarchResult.dist < 0)
            break;

        vec3 pos = ro + rd * rayMarchResult.dist;
        // Normal vector
        vec3 nor;

        if (rayMarchResult.index >= 0) {
            nor = calcSphereNormal(pos);
            finalColor += frac * render(rd, pos, nor, rayMarchResult.index);
            frac *= primitiveReflective[rayMarchResult.index];

        } else {
            nor = calcBoxNormal(pos, rd);
            finalColor += frac * renderBox(rd, pos, nor);
            frac *= faceReflective;
        }

        if (length(frac) < 0.05)
            break;

        ro = pos + nor * 0.001; // change ray origin for next bounce
        rd = reflect(rd, nor);
    }

    return finalColor;
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

    fragColor = vec4(findColor(eye, rayDirection), 1.0);

    //for debugging
//    fragColor = fragColor * 1e-6 + vec4(vec3(dbg), 1.0);
}
