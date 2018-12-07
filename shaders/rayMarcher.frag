#version 400 core

#define SPHERE 0
#define PLANE 1
#define NO_INTERSECT 2
#define DISPLACEMENT_FACTOR 0.1

// Data structure for raymarching results
struct PrimitiveDist {
    float dist;
    int primitive; // Can be SPHERE, PLANE, or NO_INTERSECT
};

// Helper function for tri3.
float tri(in float x) {
	return abs(fract(x)-.5);
}

// Triangle noise. Use it as a sample displacement map for task 7.
vec3 tri3(in vec3 p) {
    return vec3(tri(p.z+tri(p.y*1.)),
                tri(p.z+tri(p.x*1.)),
                tri(p.y+tri(p.x*1.)));
}

// TODO [Task 8] Make a displacement map
// You can check out tri3 above and the functions in the handout as inspiration
float calcDisplacement(in vec3 p) {
    vec3 noise = tri3(p)/5.0;
    return clamp(noise.x + noise.y + noise.z, -1.0, 1.0);
}


// TODO [Task 6] Implement triplanar texture mapping
// If you want, you can play around with the textures in iChannels 0 and 1
// The textures should show no distortion
vec3 texCube( sampler2D sam, in vec3 p, in vec3 n )
{
    return vec3( abs(n.x) * texture(sam, p.yz)
               + abs(n.y) * texture(sam, p.xz)
               + abs(n.z) * texture(sam, p.xy)
               );
}

// displacement resulted from texture
float texDisplace = 0.0;

// Signed distance to the twisted sphere.
float sdTwistedSphere(vec3 p) {
    vec3 spherePosition = vec3(0.0, 0.25, 0.0);
    float radius = 1.5;
    float primitive = length(p - spherePosition) - radius;
    return primitive + calcDisplacement(p) + texDisplace;
}

float sdFloor(vec3 p) {
    return p.y + texDisplace;
}

PrimitiveDist map(vec3 p) {
    // TODO [Task 3] Implement distance map
    float sd = sdTwistedSphere(p)
        , pd = sdFloor(p);

    if (sd < pd)
    	return PrimitiveDist(sd, SPHERE);
    return PrimitiveDist(pd, PLANE);
}

// TODO [Task 4] Calculate surface normals
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
        float h = map(ro + rd * marchDist).dist;
        // TODO [Task 7] Modify the loop to implement soft shadows
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

    // TODO [Task 2] Implement ray marching algorithm
    // Fill in parameters
    float marchDist = 0.001;
    float boundingDist = 50.0;
    float threshold = 0.001;

    // Fill in the iteration count
    PrimitiveDist ret = PrimitiveDist(-1.0, NO_INTERSECT);
    for (int i = 0; i < 1000; i++) {
        // Fill in loop body
        PrimitiveDist pd = map(ro + marchDist * rd);
        if (pd.dist < threshold) {
            ret.dist = marchDist;
            ret.primitive = pd.primitive;
            break;
        }

        marchDist += 0.2 * pd.dist;
        if (marchDist > boundingDist)
            break;
    }

    return ret;
}

float grayscale(vec3 c) {
	return 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
}

vec3 render(vec3 ro, vec3 rd, float t, int which) {

    vec3 pos = ro + rd * t;
    vec3 nor = calcNormal(pos);

    // TODO [Task 5] Assign different intersected objects with different materials
    // Make things pretty!
    vec3 material = vec3(0.0);
    if (which == PLANE) {
        material = texCube(iChannel0, pos, nor);
        texDisplace = grayscale(material) - .5;

    } else if (which == SPHERE) {
        material = texCube(iChannel1, pos, nor);
        texDisplace = grayscale(material) - .5;

    } else {
        material = vec3(0.5);
    }

    // Col is the final color of the current pixel.
    vec3 col = vec3(0.);
    pos = ro + rd * (t + texDisplace);
    // Light vector
    vec3 lig = normalize(vec3(1.0,0.6,0.5));

    // Normal vector
    nor = calcNormal(pos);

    // Ambient
    float ambient = 0.1;
    // Diffuse
    float diffuse = clamp(dot(nor, lig), 0.0, 1.0);
    // Specular
    float shineness = 32.0;
    float specular = pow(clamp(dot(rd, reflect(lig, nor)), 0.0, 1.0), shineness);

    float darkness = shadow(pos, lig, 18.0);
    // Applying the phong lighting model to the pixel.
    col += vec3(((ambient + diffuse + specular) * darkness));
    // Blend the material color with the original color.
    col = mix(col, material, 0.4);

    return col;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    vec3 rayOrigin = vec3(6.0 * sin(iTime * .3), 4.8, 6.0 * cos(iTime * .3));

    float focalLength = 2.0;

    // The target we are looking at
    vec3 target = vec3(0.0);
    // Look vector
    vec3 look = normalize(rayOrigin - target);
    // Up vector
    vec3 up = vec3(0.0, 1.0, 0.0);

    // Set up camera matrix
    vec3 cameraForward = -look;
    vec3 cameraRight = normalize(cross(cameraForward, up));
    vec3 cameraUp = normalize(cross(cameraRight, cameraForward));

    // TODO [Task 1] Construct the ray direction vector
    vec2 uv = fragCoord / vec2(iResolution) * 2.0 - vec2(1.0);
    uv.x *= iResolution.x / iResolution.y;

    vec3 rayDirection = vec3(uv, focalLength);
    rayDirection = rayDirection.x * cameraRight
                 + rayDirection.y * cameraUp
                 + rayDirection.z * cameraForward;
    rayDirection = normalize(rayDirection);

    PrimitiveDist rayMarchResult = raymarch(rayOrigin, rayDirection);
    vec3 col = vec3(0.0);
    if (rayMarchResult.primitive != NO_INTERSECT) {
      	col = render(rayOrigin, rayDirection, rayMarchResult.dist, rayMarchResult.primitive);
    }

    fragColor = vec4(col, 1.0);
}
