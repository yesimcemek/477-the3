#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 projection; // ModelViewProjection Matrix
uniform mat4 view; // ModelView idMVPMatrix
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float heightFactor;
uniform int textureOffset;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

float computeHeight(vec2 pos)
{
	return (texture(rgbTexture, pos).x)*heightFactor;
}

vec3 computeNormal(vec3 a, vec3 b, vec3 c)
{
	vec3 normal = normalize(cross(b - a, c - a));
	return normal;
}

void main()
{

    // get texture value, compute height
    vec3 pos;
    pos.x = position.x;
    pos.z = position.z;

    float dede = pos.x + textureOffset;
    if (dede > widthTexture){
    	dede -= widthTexture;
    }

    textureCoordinate = vec2(1 - dede/widthTexture, 1 - pos.z/heightTexture);
    pos.y = computeHeight(textureCoordinate);
    
     // compute toLight vector vertex coordinate in VCS
   	ToLightVector = normalize(lightPosition - pos);
   	ToCameraVector = normalize(cameraPosition - pos);
   	
   // set gl_Position variable correctly to give the transformed vertex position

   gl_Position = projection*view*vec4(pos, 1.0);
    // compute normal vector using also the heights of neighbor vertices
    float x = position.x;
    float z = position.z;
    if(x > 0 && x < widthTexture && z > 0 && z < heightTexture) 
	{
		vec3 top_right = vec3(x + 1, 0, z - 1);
		top_right.y = computeHeight(vec2(1 - dede/widthTexture, 1 - top_right.z/heightTexture));
		
		vec3 top = vec3(x, 0, z - 1);
		top.y = computeHeight(vec2(1 - dede/widthTexture, 1 - top.z/heightTexture));
		
		vec3 left = vec3(x - 1, 0, z);
		left.y = computeHeight(vec2(1 - dede/widthTexture, 1 - left.z/heightTexture));
		
		vec3 bottom_left = vec3(x - 1, 0, z + 1);
		bottom_left.y = computeHeight(vec2(1 - dede/widthTexture, 1 - bottom_left.z/heightTexture));
		
		vec3 bottom = vec3(x, 0, z + 1);
		bottom.y = computeHeight(vec2(1 - dede/widthTexture, 1 - bottom.z/heightTexture));
		
		vec3 right = vec3(x + 1, 0, z);
		right.y = computeHeight(vec2(1 - dede/widthTexture, 1 - right.z/heightTexture));
		
		vec3 normal1 = computeNormal(pos, top_right, top);
		vec3 normal2 = computeNormal(pos, top, left);
		vec3 normal3 = computeNormal(pos, left, bottom_left);
		vec3 normal4 = computeNormal(pos, bottom_left, bottom);
		vec3 normal5 = computeNormal(pos, bottom, right);
		vec3 normal6 = computeNormal(pos, right, top);
		
		vertexNormal = normalize((normal1 + normal2 + normal3 + normal4 + normal5 + normal6)/6);
	}
	else
	{
		vertexNormal = vec3(0, 0, 0);
	}
	
    
    
}
