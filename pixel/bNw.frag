// Black and White

uniform sampler2D img;

vec4 sample(float dx,float dy)
{
   return texture2D(img,gl_TexCoord[0].st+vec2(dx,dy));
}

void main()
{
   float one = 1.0/3.0;
   vec4 colors = sample(0.0, 0.0);
   float comp = one*colors.r + one*colors.g + one*colors.b;

   gl_FragColor = vec4(comp, comp, comp, 1.0);
}
