// Smooth (low-pass)
//   1 2 1
//   2 6 2   / 18
//   1 2 1

uniform float dX;
uniform float dY;
uniform sampler2D img;

vec4 sample(float dx,float dy)
{
   return texture2D(img,gl_TexCoord[0].st+vec2(dx,dy));
}

void main()
{
   float one = 1.0/18.0;
   float two = 2.0/18.0;
   float six = 6.0/18.0;
   gl_FragColor = one*sample(-dX,+dY) + two*sample(0.0,+dY) + one*sample(+dX,+dY)
                + two*sample(-dX,0.0) + six*sample(0.0,0.0) + two*sample(+dX,0.0)
                + one*sample(-dX,-dY) + two*sample(0.0,-dY) + one*sample(+dX,-dY);
}
