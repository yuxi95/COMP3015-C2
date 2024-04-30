#version 330 core

out vec4    FragColor;
in vec2     TexCoords;
in vec3     WorldPos;
in vec3     Normal;
in vec3     Tanget;
in vec3     Binormal;
in vec4     lsPos;

uniform sampler2D   albedoMap;
uniform sampler2D   shadowMap;
uniform sampler2D   projMap;
uniform sampler2D   normalMap;

uniform vec3        LightDir;
uniform vec3        camPos;

void main()
{		
    vec4 albedo = texture(albedoMap, TexCoords);
    if (albedo.a==0.)
        discard;

    vec3 L = normalize(-LightDir);
    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);


    vec3 T = normalize(Tanget);
    vec3 B = normalize(Binormal);
    mat3 TBN = mat3(T, B, N);
    N = texture(normalMap, TexCoords).rgb;
    N = N * 2.0 - 1.0;   
    N = normalize( TBN * N ); 




    // perform perspective divide
    vec3 proj_coords = lsPos.xyz / lsPos.w;
  
    vec3 proj_color = texture( projMap, (( (proj_coords +  vec3(0.35,0,0) ) * 8.0) * 0.5 + 0.5) .xy).xyz; 


    // transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5;
    


    float current_depth = proj_coords.z;

  
    float bias = 0;// max(0.05 * (1.0 - dot(N, L)), 0.005);  

    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, proj_coords.xy + vec2(x, y) * texelSize).r; 
            shadow += current_depth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    float fshadow =  (1.0- shadow) *  max(dot(N,L), 0 ) ;

    // float backface = (dot(L,N) < 0.0 ? 0.0 : 1 );

    FragColor = vec4 ( ( 0.25 + 0.75 * fshadow) * albedo.rgb * proj_color, 0.0) ;

    
}
