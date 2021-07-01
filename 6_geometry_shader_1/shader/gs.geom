#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

uniform vec4 offset_val[5] = vec4[](
                                    vec4(-0.2, -0.2, 0.0, 0.0),
                                    vec4( 0.2, -0.2, 0.0, 0.0),
									vec4(-0.2,  0.2, 0.0, 0.0),
									vec4( 0.2,  0.2, 0.0, 0.0),
                                    vec4( 0.0,  0.4, 0.0, 0.0)
								    );

void main(){
  //int first_val = gs_in[0].color.x;
  if(gs_in[0].color.x == 0.0f){   //  square, the primitive type is triangle strip, so 5 points is need to draw a square.
	fColor = gs_in[0].color;
    gl_Position = gl_in[0].gl_Position + vec4(0.0, -0.2, 0.0, 0.0);
    EmitVertex();
	fColor = gs_in[0].color;
    gl_Position = gl_in[0].gl_Position + vec4(0.2, -0.2, 0.0, 0.0);
    EmitVertex();
	fColor = gs_in[0].color;
    gl_Position = gl_in[0].gl_Position + vec4(0.2, 0.2, 0.0, 0.0);
    EmitVertex();
	fColor = gs_in[0].color;
    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.2, 0.0, 0.0);
    EmitVertex();
	fColor = gs_in[0].color;
    gl_Position = gl_in[0].gl_Position + vec4(0.0, -0.2, 0.0, 0.0);
    EmitVertex();
    return;
  }else{
    if(gs_in[0].color.y == 1.0f){
      if(gs_in[0].color.z <= 0.5f){  // pentagon
        for(int i = 0; i < 5; i++){
		  fColor = gs_in[0].color;
		  gl_Position = gl_in[0].gl_Position + offset_val[i];
          EmitVertex();
        }  
        return;   
      }else{   //  triangle
        fColor = vec3(1.0, 0, 0);
		gl_Position = gl_in[0].gl_Position + vec4(0.2, -0.2, 0.0, 0.0);
        EmitVertex();
		fColor = vec3(1.0, 0, 0);
		gl_Position = gl_in[0].gl_Position + vec4(0, 0.2, 0.0, 0.0);
        EmitVertex();
		fColor = vec3(1.0, 0, 0);
		gl_Position = gl_in[0].gl_Position + vec4(-0.2, -0.2, 0.0, 0.0);
        EmitVertex();
      }
    }else{
      return;   //  这里比较奇怪
	  //EmitVertex()
    }
  }
}
