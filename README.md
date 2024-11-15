# Kwan's Ray Tracer

personal experiment project

## Notes
### Graphics
- visualize normal: 
    - for each point P on the surface, get unit normal vector (P - C) (each component -1 ≤ e ≤ 1)
    - map each component to 0 ≤ e ≤ 1 (by (e+1) / 2)
    - each component to r, g, b
### Maths
- lerp: linear interpolation, blend = (1-a)*start + a*end
- vector dot product: a • b = |a|*|b|*cos(t), graphically |a| * (the projection of b on a)
- vector cross product: a X b = |a|*|b|*sin(t)*n, n follows the right hand rule (a=index finger, b=middle finger, n=thumb)
    - cross product of itself yields 0, b/c sin(0) == 0
    - to resolve equation with cross products on both sides, dot product another vector on both sides
    - a X b = -b X a
- radians & degrees: rad = deg * pi / 180 (arc length of unit circle)
### V1
#### Render Output
- defines the aspect ratio
- [used] right-handed coordinate (in this case) x-right, y-up, z-back(away from the viewport)
- format: ppm, or png with lib
- gamma correction for accurate color intensity: map our data (linear data) to gamma space, by inverse gamma 2 (negative: 0, others: square root)
#### Camera
- config
    - aspect ratio
    - img width
    - samples per pixel (for antialiasing)
    - max depth
    - lensCenter & viewportCenter & vUp (a vector always pointing up (0, 1, 0))
        - used to define camera orientation u(right), v(up), w(back)
        - w = unitVec(lensCenter - viewportCenter)
        - u = vUp X w
        - v = w X u
    - focus distance (length between lensCenter and focus plane)
        - focal length (length between lensCenter and viewportCenter, same as focus distance in this model)
    - defocus angle (why not use apperture radius directly?)
        - apertureRadius = focusDistance * tan(defocusAngle / 2)
        - use apertureRadius to randomly offset origin of rays firing from the lens
    - fov (an angle, usually just define 1 direction, using vertical fov)
        - viewportHeight = focalLen * tan(vfov / 2) * 2
- public method
    - render(world)
        - iterate each pixel (square, disk, ...), fire multiple randomly offset rays and sum the resulting colors from rayColor()
            - can parallelize?
    - init()
        - prepares viewport
            - viewport U/V: viewportU = viewportWidth * u, viewportV = -viewportHeight * v (points down)
            - viewport pixel delta vectors: pixelDeltaU = viewportU / imgWidth, pixelDeltaV = viewportV / imgHeight
            - starting pixel [top-left] lensCenter - focalLength * w - viewportU/2 - viewportV/2
- private method
    - rayColor
        - if miss, return black; otherwise, get material of the hit obj, run scatter, if scatterred, color is the returned attenuation * rayColor(scattered ray)
        - recursive call, limit by max depth
        - shadow acne (surface self-shadowing): can be fixed by adding a small offset to tInterval.min instead of 0
#### Viewport
- square pixel for simplicity
- a ray originates from the camera to the center of each pixel of the viewport, and define color based on the ray
#### hitRecord: used to record hit, ray, material for coloring
- records a ray-object intersection
- records hit point, normal, t, bool frontFacing (see #facing), hit material (shared ptr to avoid circular ref)
#### World (geometry)
- facing
    - to determine front/back faces (dot product of ray and outwardNormal), do any one of the following:
        - assume all normals are front facing, then decide front/back when coloring (good for more geometries than materials)
        - [used here] assume the normal always go against the ray, then save frontFace bool at the hitRecord (good for more materials than geometries)
- base class - hittable
    - check hit with a ray and a tInterval, and save any hit in a hitRecord
- hittableList
    - derived from hittable; contains a list of hittable
    - check hit: iterate hittables, adjust tInterval.max with each hit, and keep track of the nearest hitRecord
- sphere
    - ray sphere-intersection
```
P: any point, C: sphere center, (C-P)•(C-P)=r^2, if there's solution, then it's a hit
P = O + td, O: ray origin, d: ray direction, t: the variable

In the end it's down to the solve of a quadratic formula, and we just need to verify if there's any solution
discriminant = b^2 - 4*a*c ≥ 0 <--> hit
OC = (C - O), a = d • d, b = -2d • OC, c = OC • OC - r^2

We can simplify furthermore: d • d is squared len of vector d, b = -2*h
a = d.squareLen(), h = d • OC, c = OC.squareLen() - r^2
discriminant = 4 * (h^2 - a * c)
if solvable, t = (-b +- √discriminant) / 2*a = (h +- √(h^2 - a * c)) / a

first try the smaller t and see if it's within tInterval (boundaries not included), if not then try the bigger one
```
#### Material
- base class
    - scatter: take a ray, decide if reflect, and the reflection attenuation
- Lambertian (diffuse)
    - the reflection should follow the Lambertian cosine law; the reflected rays should distribute evenly on a unit sphere tangent to the hit point
    - so we gen (a random unit vector pointing outwards + outward unit normal) as the reflected ray, and multiply a reflectance/attenuation/albedo with the color of the reflection ray
        - beware of vector near 0 for all components; in that case, just set reflection direction to normal
        - gen random vector with components within (-1, 1) ! (otherwise we'll gen something like failed-cornell-box.ppm)
    - random unit vector: randomly generate points within a unit cube, and continue until a point within the unit sphere is found
        - beware of points with small radius (limit the min radius)
- Metal (mirrored reflection)
    - reflection = inRay - 2 * inRayProjectionOnNormal = inRay - 2 * (inRay • n) * n
    - fuzzy reflection: normalize relection, add another small(len≤1) fuzzy vector to the reflection, and only reflect if dot(outRay, normal) > 0
- Dielectrics (reflection and refraction)
    - randomly reflect or refract (realistically ray will split into a reflection and a refraction)
    - usually we define a relative refraction index for the material, which is internalRefractionIdx / externalRefractionIdx
    - implementation
        - define ri: if frontFacing, ri = 1 / relRi, otherwise ri = relRi
        - cannotRefract = ri * sin(tIn) > 1 (from Snell's Law)
        - if (cannotRefract || reflectance > randomDouble()), reflect like metal
            - see Schlick Approx for reflectance calculation
        - else refract based on Snell's Law
    - Snell's Law: etaIn * sin(tIn) = etaOut * sin(tOut), eta: refraction index (air: 1, glass: 1.5)
        - solution
            - outRayPerpToNormal = ri * (inRay + cos(tIn) * normal)
                - replace cos(tIn) with -dot(unitInRay, unitNormal)
            - outRayParallelToNormal = -√(1 - |outRayPerpToNormal|^2) * normal
    - reflectance
        - too complex
        - use Schlick Approx: reflectance = r0 + (1 - r0) * (1 - cos(tIn))^5
            - r0 = ((1 - ri) / (1 + ri))^2
    - hint: use dot product to replace cosine, and use sqrt(1 - cosine^2) to replace sine
            
### V2
#### space-time
- define a duration of a frame
- add a member (time) to ray to mark the time of the ray
- camera sends each ray at a random time within the frame duration (can simplify to [0, 1])
- implementations of hittable should use ray time to get its realtime location
- material scattered light should inherit the inRay time
#### Bounding Volume Hierarchies
- divide and conquer to speed up the hit detection
- Hittable: add a common member AABB bBox (boundingBox)
- BVH node (derived from Hittable)
    - [constructor] split a list of hittables into left and right child nodes, and create bBox
        - 1 hittable: left and right both points to it
        - 2 hittables: left and right points to one respectively
        - more: choose the longest axis, sort and divide
        - assign its bBox with an AABB to encompass the AABBs of left and right
    - [hit] (ray, tInterval, hitRecord) check if bBox hit, and then left and right hit check
        - if left hit, tInterval.max for right check should be adjusted to hitRecord.t
- AABB (axis-aligned bounding box)
    - [member] 3 intervals along 3 axes; at construction, pad intervals to avoid narrow sides (say delta=0.0001)
    - [hit] calc tIntervals along 3 axes, return true if all 3 intervals overlap
        - for axis x, t = (xt - rayOrigin.x) / rayDir.x, will plug in each end of axis interval at xt
    - moving objects: simplify by binding the start and end bounding boxes
- other hittables: no need to utilize bBox in hit()
- simple benchmarking with Scene balls (540 px width, 5 samples per pixel): 249858ms vs 64126ms
- one mistake made: Interval::empty can't be set with (inf, -inf) b/c Interval constructor will adjust the order
#### Texture Mapping
- basic idea is from a point of a geometry, trace back to find the color on the texture map
- so texture needs to know the coordinate, texture.value(u, v, point)
- add u, v to hitRecord
- Solid Color Texture
    - irrelevant to the coordinate, always return albedo
- Spatial Checkered Texture
    - sumOf3Dimensions( (floor(inverseScale * dimensionComponent)) ) % 2 -> odd or even texture
- Image Texture
    - texture coordinates for sphere
        - u, v: range [0, 1] from bottom left corner of an img, mapped from phi (angle from -X counter clockwise) and theta (angle up from -Y)
        - u = phi / (2*pi), v = theta / pi; y = -cos(theta), x = sin(theta) * (-cos(phi)), z = sin(theta) * sin(phi)
        - phi = atan2(-z, x) + pi, theta = arccos(-y)
    - load image from file
        - use stbi_loadf to load (include "stb_image.h" from https://github.com/nothings/stb) as float ([0, 1]) array
        - convert float array to byte array (unsigned char [0, 255])
        - main method: pixelData(row, col), returns the address of R (in RGB) in the byte array
    - value(u, v, point)
        - in the end, scale to Color with all 3 components in range [0, 1]
#### Perlin Noise
- combined with sine, can create marble like texture, or white noise
- steps of improvement
    - hashed
    - smooth with lerp
    - round off lerp with Hermite cubic
    - tweak freq with scale
    - random vectors
    - turbulence (sum noises of a series of frequencies)
    - use turbulence to adjust the phase of a sine
- final implementation
    - perlin
        - [randVecArr] static random unit Vec3 (-1, 1) array of size 256
        - [permArr] static int array of size 256 for each axis with shuffled indices
        - [double (*noise)(Point p)]
            - split each component of p to integer and decimal
            - use XOR of processed integers to choose random vector from randVecArr, and gen a 3-by-2 matrix
            - lerp (with Hermitian smoothing) matrix with decimals
        - [double (*turbulence)(Point p, int depth)]
            - acc += weight * noise(tempP) for 'depth' iterations
            - weight *= 0.5, starts from 1
            - tempP *= 2, starts from p
            - return abs(acc)
    - noise texture
        - [Color (*value)(double u, double v, Point point)]
            - gray color * (1 + sine(scale * point.z + 10 * perlinNoise.turbulence(point, 7)))
#### 2D Primitive
- Quad
    - defined by a point Q and two non-parallel vectors u, v
    - [rayPlaneIntersection] A*x + B*y + C*z = D -> un • p = D
        - un is unit normal vector of the plane, un = cross(u, v).unitVec()
        - we can use Q to get D -> D = un • Q
        - p is a point on ray -> p = O + t*d -> un • (O + t*d) = D -> t = (D - un • O) / (un • d)
            - if un • d is very close 0, take it as a miss
            - if t is outside of tInterval, miss
    - [isInterior] point on plane p = Q + alpha*u + beta*v, u & v might not be orthogonal
        - p - Q = QP (vec Q to p) = alpha*u + beta*v
        - cross product u and v: cross(u, QP) = beta*cross(u, v), cross(v, QP) = alpha*cross(v, u)
        - dot product n (cross(u, v), not unit!) to eliminate cross products
            - n • cross(u, QP) = beta * n • cross(u, v) = beta * n • n
            - n • cross(v, QP) = alpha * n • cross(v, u) = -alpha * n • cross(u, v) = -alpha * n • n
        - beta = (n • cross(u, QP)) / n • n, alpha = -(n • cross(v, QP)) / n • n
        - let w = n / n • n, alpha = w • cross(QP, v), beta = w • cross(u, QP)
        - for quad, alpha and beta within [0, 1]
        - we can expand this method to support other 2D primitives
#### Light
- [Color(*emit)(u, v, Point)] for all materials
- camera rayColor: emit if hit but not scattered; add scattered and emitted as output
- albedo of light should be greater than 1 light other objects
#### Instance
change of coordinates: change the ray from world space to obj space, check ray hit, change the intersection (if any) back to world space
- Translation
    - [member] a ptr to a hittable, a Point offset
    - [constructor] bbox = hittable's bbox + offset
    - [hit] offsetRay = ray - offset, check hit; if hit, add offset to hitPoint
- Rotation
    - [_Rotation on Axes_](https://math.sci.ccny.cuny.edu/document/Rotation+of+Axes)
        - for rotation about y: newX = cos(theta) * x + sin(theta) * z, newZ = -sin(theta) * x + cos(theta) * z
        - rotation of a ray: both origin and direction need to be rotated
    - [member] a ptr to a hittable, a angle to rotate on an axis
    - [constructor]
        - get the bbox of the hittable
        - iterate through all 8 vertices of bbox, rotate each vertex backwards, and keep a record of two points minP and maxP (max x, y, z)
        - set bbox with minP and maxP
    - [hit] rotate the ray backwards, check hit; if hit, rotate the hitPoint and normal
- Scaling (TODO)
#### Volume
the ray scatters based on probability = C * deltaLengthTravelled
for simplicity, assume that the ray won't re-enter the volume once left (?)
- [hittable] constant density medium
    - [constructor]
        - a hittable as boundary, density, albedo/texture
        - set material = isotropic
    - [hit]
        - try hit twice with (-inf, inf) and (tHit1, inf)
        - clamp (tHi1, tHit2) with (tMin, tMax) and [0, inf]
        - distanceInBoundary = rayLen * (tHit2 - tHit1)
        - [why?] hitDistance = negativeInverseDensity * log(randomDouble())
        - hit if hitDistance <= distanceInBoundary, t = tHit1 + hitDistance / rayLen
        - set normal = (1,0,0) and frontFacing = true (arbitary, ?)
- [material] isotropic
    - always scatter, but the direction is random
## Miscellaneous
- install brew (mainland China): ```/bin/zsh -c "$(curl -fsSL https://gitee.com/cunkai/HomebrewCN/raw/master/Homebrew.sh)"```

## Reference & Special Thanks
[_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)

[_Ray Tracing: The Next Week_](https://raytracing.github.io/books/RayTracingTheNextWeek.html)

[_Ray Tracing: The Rest of Your Life_](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html)
