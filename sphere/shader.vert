#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 projection; // ModelViewProjection Matrix
uniform mat4 view; // ModelView idMVPMatrix
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D heightTex;  // The height  (we'll bind to texture unit 0)
uniform sampler2D normalTex;   // The texture (we'll bind to texture unit 1)

uniform int widthTexture;
uniform int heightTexture;
uniform float radius;
uniform float pi;
uniform float textureOffset;
int horizontalSplitCount = 250;
int verticalSplitCount = 125;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

float computeHeight(vec2 pos)
{
	//return (((texture(rgbTexture, pos).x *0.2126) + (0.7152 * texture(rgbTexture, pos).y) + (0.0722 * texture(rgbTexture, pos).z)) * heightFactor);
	return (texture(heightTex, pos).x)*heightFactor;
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
	//float offset = mod(textureOffset, 250)/250.0;
	float offset = textureOffset * (2 * pi);
	alpha = alpha + offset;
	if (alpha > 2 * pi){
		alpha = alpha - (2 * pi);
	}
    
	return vec2(alpha/(2*pi), beta/pi);
}
vec3 computePoint(int horizontalStep, int verticalStep)
{
	float alpha = 2*pi*horizontalStep/horizontalSplitCount;
	float beta = pi*verticalStep/verticalSplitCount;
	float x = radius*sin(beta)*cos(alpha);
	float y = radius*sin(beta)*sin(alpha);
	float z = radius*cos(beta);
	
	return vec3(x, y, z);
}

void main()
{

	// get texture value, compute height
	vec3 pos;
	pos.x = position.x;
	pos.z = position.z;
	pos.y = position.y;

	textureCoordinate = computeCoord(pos);
	//pos.y = computeHeight(textureCoordinate);

	// compute toLight vector vertex coordinate in VCS
	ToLightVector = normalize(lightPosition - pos);
	ToCameraVector = normalize(cameraPosition - pos);

	//set gl_Position variable correctly to give the transformed vertex position

	gl_Position = projection*view*vec4(pos, 1.0);
	float beta = acos(pos.z/radius);
    	float alpha = acos(pos.x/(radius*sin(beta)));
	int horizontalStep = int(round((alpha*horizontalSplitCount)/(2*pi)));
	int verticalStep = int(round((beta*verticalSplitCount)/(pi)));
	
	vec3 top = computePoint(horizontalStep, verticalStep - 1);
	vec3 top_right = computePoint(horizontalStep + 1, verticalStep - 1);
	vec3 right = computePoint(horizontalStep + 1, verticalStep);
	vec3 bottom = computePoint(horizontalStep, verticalStep + 1);
	vec3 bottom_left = computePoint(horizontalStep - 1, verticalStep + 1);
	vec3 left = computePoint(horizontalStep - 1, verticalStep);
	
	vec3 normal1 = computeNormal(pos, top_right, top);
	vec3 normal2 = computeNormal(pos, top, left);
	vec3 normal3 = computeNormal(pos, left, bottom_left);
	vec3 normal4 = computeNormal(pos, bottom_left, bottom);
	vec3 normal5 = computeNormal(pos, bottom, right);
	vec3 normal6 = computeNormal(pos, right, top);
	
	vertexNormal = normalize((normal1 + normal2 + normal3 + normal4 + normal5 + normal6)/6);



    
    
}
