//Identity vertex shader
uniform vec4 location;
varying vec4 vertex;

void main()
{
 //gl_FrontColor = gl_Color;
 //Set vertex coordinates
 //vertex = gl_Color;
   vertex = gl_Vertex + location;
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
