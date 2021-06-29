## Note

​    this case is extracted from LearnOpengl tutorial, but in the geometry shader, the opengl language spec says that after EmitVertex() is called, the value of output variables are undefined. Intel gpu choose to keep the last value of output variables, so we can see every object has its own color, but it is not always the case.

