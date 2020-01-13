#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 projection; // ModelViewProjection Matrix
uniform mat4 view; // ModelView idMVPMatrix
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;
uniform float radius;
uniform float pi;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

float computeHeight(vec2 pos)
{
	//return (((texture(rgbTexture, pos).x *0.2126) + (0.7152 * texture(rgbTexture, pos).y) + (0.0722 * texture(rgbTexture, pos).z)) * heightFactor);
	return (texture(rgbTexture, pos).x)*heightFactor;
}

vec3 computeNormal(vec3 a, vec3 b, vec3 c)
{
	vec3 normal = normalize(cross(b - a, c - a));
	return normal;
}

vec2 computeCoord(vec3 pos)
{
	float beta = acos(pos.z/radius);
    	float alpha = acos(pos.x/(radius*sin(beta)));
    
	return vec2(alpha/(2*pi), beta/pi);
}

void main()
{

    // get texture value, compute height
    vec3 pos;
    pos.x = position.x;
    pos.z = position.z;

    textureCoordinate = computeCoord(pos);
    pos.y = computeHeight(textureCoordinate);
    
     // compute toLight vector vertex coordinate in VCS
   	ToLightVector = normalize(lightPosition - pos);
   	ToCameraVector = normalize(cameraPosition - pos);
   	
   // set gl_Position variable correctly to give the transformed vertex position

   gl_Position = projection*view*vec4(pos, 1.0);
    // compute normal vector using also the heights of neighbor vertices
    float x = position.x;
    float z = position.z;
	vertexNormal = normalize(pos);

	
    
    
}
