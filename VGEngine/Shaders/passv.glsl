const char *passv = STRINGIFY(
void main(void)
{
  // Transforming The Vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
    gl_TexCoord[0] = gl_MultiTexCoord0;

}
);
