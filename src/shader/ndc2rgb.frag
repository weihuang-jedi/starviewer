//Set the fragment color
varying vec4 vertex;

void main()
{
  //gl_FragColor = gl_Vertex + vec4(0.5, 0.5, 0.5, 0.0);
  //gl_FragColor = gl_Color;
  //gl_FragColor = vertex;
    gl_FragColor = vertex + vec4(0.5, 0.5, 0.5, 0.0);
}
