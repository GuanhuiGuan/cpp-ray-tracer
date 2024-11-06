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
- radians & degrees: rad = deg * pi / 180 (arc length of unit circle)
### V1
- render output (image)
    - defines the aspect ratio
    - [used] right-handed coordinate (in this case) x-right, y-up, z-back(away from the viewport)
    - format: ppm, or png with lib
    - gamma correction for accurate color intensity: map our data (linear data) to gamma space, by inverse gamma 2 (negative: 0, others: square root)
- camera
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
- viewport
    - usually square pixel
    - a ray originates from the camera to the center of each pixel of the viewport, and define color based on the ray
- hitRecord: used to record hit, ray, material for coloring
    - records a ray-object intersection
    - records hit point, normal, t, bool frontFacing (see #facing), hit material (shared ptr to avoid circular ref)
- world (geometry)
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
- material
    - base class
        - scatter: take a ray, decide if reflect, and the reflection attenuation
    - Lambertian (diffuse)
        - the reflection should follow the Lambertian cosine law; the reflected rays should distribute evenly on a unit sphere tangent to the hit point
        - so we gen (a random unit vector pointing outwards + outward unit normal) as the reflected ray, and multiply a reflectance/attenuation/albedo with the color of the reflection ray
            - beware of vector near 0 for all components; in that case, just set reflection direction to normal
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
- space-time
    - define a duration of a frame
    - add a member (time) to ray to mark the time of the ray
    - camera sends each ray at a random time within the frame duration (can simplify to [0, 1])
    - implementations of hittable should use ray time to get its realtime location
    - material scattered light should inherit the inRay time


## Miscellaneous
- install brew (mainland China): /bin/zsh -c "$(curl -fsSL https://gitee.com/cunkai/HomebrewCN/raw/master/Homebrew.sh)"
