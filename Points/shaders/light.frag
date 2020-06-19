#version 450 core
out vec4 FragColour;

in vec3 Position;
in vec3 Colour;
in vec3 Normal;  
  
uniform vec3 lightPosition; 
uniform vec3 viewPosition; 
uniform vec3 lightColour;

void main()
{
///*					  (AMBIENT)			(DIFFUSE)			(SPECULAR)
	//
	//Light equation =		KₐIₐ     +      (KᵤIₗ(N⋅L)	 +		KₛIₗ(N⋅H)ⁿˢ)  */

    // ambient
    float ambientStrength = 0.2;

    vec3 ambient = ambientStrength * lightColour;
  	
    // diffuse 
    vec3 normal = normalize(Normal);

    vec3 lightDirection = normalize(lightPosition - Position);

    float d = max(dot(normal, lightDirection), 0.0);

    vec3 diffuse = d * lightColour;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(viewPosition - Position);
    vec3 reflectDirection = reflect(-lightDirection, normal);  
    float s = pow(max(dot(viewDirection, reflectDirection), 0.0), 16);
    vec3 specular = specularStrength * s * lightColour;  
        
    vec3 result = (ambient + diffuse + specular) * Colour;
    FragColour = vec4(result, 1.0f);
    
}
