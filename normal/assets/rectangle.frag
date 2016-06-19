precision highp float;						   		  
uniform sampler2D sampler2d_image;   // original image			              
uniform sampler2D sampler2d_front;   // lookup table of front                  
uniform sampler2D sampler2d_rear;			              
uniform sampler2D sampler2d_left;			              
uniform sampler2D sampler2d_right;			              
uniform sampler2D sampler2d_car;	// a car texture
													  
void main(void)                                          
{                                                        
	highp float frontweight,rearweight,leftweight,rightweight;    
	highp float frontsrcx,frontsrcy,rearsrcx,rearsrcy;          
	highp float leftsrcx,leftsrcy,rightsrcx,rightsrcy;	 			 
	vec4 frontwvec,rearwvec,leftwvec,rightwvec;			  
	vec4 frontval,rearval,leftval,rightval;				  
	vec4 tmpfrontxvec,tmpfrontyvec,tmprearxvec,tmprearyvec;
	vec4 tmpleftxvec,tmpleftyvec,tmprightxvec,tmprightyvec;

	// display rear image

	if(gl_FragCoord.x>=305.0)
	{
		gl_FragColor = texture2D(sampler2d_image,vec2((gl_FragCoord.x-305.0)/830.0+0.5,0.5-gl_FragCoord.y/960.0));
	}
	
	//display car texture
	
	else if(gl_FragCoord.x>=110.0&&gl_FragCoord.x<=210.0&&gl_FragCoord.y>=190.0&&gl_FragCoord.y<=300.0)
	{
	
	//	gl_FragColor = texture2D(sampler2d_car,vec2((gl_FragCoord.x-110.0)/100.0,(gl_FragCoord.y-190.0)/110.0));
		
		gl_FragColor = texture2D(sampler2d_car,vec2(1.0-(gl_FragCoord.y-190.0)/110.0,1.0-(gl_FragCoord.x-110.0)/100.0));
	}

	// generate panorama

	else if(gl_FragCoord.x<=300.0)
	{

	// restore an ineger in cpu from four bytes 

	frontwvec=texture2D(sampler2d_front,vec2((gl_FragCoord.x+300.0*2.0)/900.0,(480.0-gl_FragCoord.y)/480.0));										  
	rearwvec=texture2D(sampler2d_rear,vec2((gl_FragCoord.x+300.0*2.0)/900.0,(480.0-gl_FragCoord.y)/480.0));											  
	leftwvec=texture2D(sampler2d_left,vec2((gl_FragCoord.x+300.0*2.0)/900.0,(480.0-gl_FragCoord.y)/480.0));											  
	rightwvec=texture2D(sampler2d_right,vec2((gl_FragCoord.x+300.0*2.0)/900.0,(480.0-gl_FragCoord.y)/480.0));										  
															  
	frontweight=frontwvec.x*255.0+frontwvec.y*255.0*256.0;			  
	rearweight=rearwvec.x*255.0+rearwvec.y*255.0*256.0;				  
	leftweight=leftwvec.x*255.0+leftwvec.y*255.0*256.0;	  
	rightweight=rightwvec.x*255.0+rightwvec.y*255.0*256.0;	  
															  
	if(frontweight<1.0)									  
		frontval=vec4(0.0,0.0,0.0,0.0);					  
	else												  
	{	
		// the x and y coordinate has been mutiplied by 1024 before

		tmpfrontxvec=texture2D(sampler2d_front,vec2(gl_FragCoord.x/900.0,(480.0-gl_FragCoord.y)/480.0));										  
		tmpfrontyvec=texture2D(sampler2d_front,vec2((gl_FragCoord.x+300.0)/900.0,(480.0-gl_FragCoord.y)/480.0));								  
		frontsrcx=tmpfrontxvec.x*0.249+tmpfrontxvec.y*63.75+tmpfrontxvec.z*16320.0;	 
		frontsrcy=tmpfrontyvec.x*0.249+tmpfrontyvec.y*63.75+tmpfrontyvec.z*16320.0;	 
		frontval=texture2D(sampler2d_image,vec2(frontsrcx/1440.0,frontsrcy/960.0));								  
	}													  
															  
	if(rearweight<1.0)									  
		rearval=vec4(0.0,0.0,0.0,0.0);					  
	else												  
	{													  
		tmprearxvec=texture2D(sampler2d_rear,vec2(gl_FragCoord.x/900.0,(480.0-gl_FragCoord.y)/480.0));											  
		tmprearyvec=texture2D(sampler2d_rear,vec2((gl_FragCoord.x+300.0)/900.0,(480.0-gl_FragCoord.y)/480.0));									  
		rearsrcx=tmprearxvec.x*0.249+tmprearxvec.y*63.75+tmprearxvec.z*16320.0;	
		rearsrcy=tmprearyvec.x*0.249+tmprearyvec.y*63.75+tmprearyvec.z*16320.0;	
		rearval=texture2D(sampler2d_image,vec2(rearsrcx/1440.0,rearsrcy/960.0));
	}													  
	if(leftweight<1.0)									  
		leftval=vec4(0.0,0.0,0.0,0.0);					  
	else												  
	{													  
		tmpleftxvec=texture2D(sampler2d_left,vec2(gl_FragCoord.x/900.0,(480.0-gl_FragCoord.y)/480.0));											  
		tmpleftyvec=texture2D(sampler2d_left,vec2((gl_FragCoord.x+300.0)/900.0,(480.0-gl_FragCoord.y)/480.0));									  
		leftsrcx=tmpleftxvec.x*0.249+tmpleftxvec.y*63.75+tmpleftxvec.z*16320.0;	
		leftsrcy=tmpleftyvec.x*0.249+tmpleftyvec.y*63.75+tmpleftyvec.z*16320.0;	
		leftval=texture2D(sampler2d_image,vec2(leftsrcx/1440.0,leftsrcy/960.0));
	}													  
	if(rightweight<1.0)									  
		rightval=vec4(0.0,0.0,0.0,0.0);					  
	else												  
	{													  
		tmprightxvec=texture2D(sampler2d_right,vec2(gl_FragCoord.x/900.0,(480.0-gl_FragCoord.y)/480.0));										  
		tmprightyvec=texture2D(sampler2d_right,vec2((gl_FragCoord.x+300.0)/900.0,(480.0-gl_FragCoord.y)/480.0));									  
		rightsrcx=tmprightxvec.x*0.249+tmprightxvec.y*63.75+tmprightxvec.z*16320.0;		  
		rightsrcy=tmprightyvec.x*0.249+tmprightyvec.y*63.75+tmprightyvec.z*16320.0;		  
		rightval=texture2D(sampler2d_image,vec2(rightsrcx/1440.0,rightsrcy/960.0));								  
	}												  
	gl_FragColor.x=(frontweight*frontval.x+rearweight*rearval.x+leftweight*leftval.x+rightweight*rightval.x)/256.0;						  
	gl_FragColor.y=(frontweight*frontval.y+rearweight*rearval.y+leftweight*leftval.y+rightweight*rightval.y)/256.0;						  
	gl_FragColor.z=(frontweight*frontval.z+rearweight*rearval.z+leftweight*leftval.z+rightweight*rightval.z)/256.0;						  
	gl_FragColor.w=1.0;							
	}

	//otherwise the color is black

	else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,1.0);
	}
}													      

