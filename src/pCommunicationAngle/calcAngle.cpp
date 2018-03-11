
calcAngle::getTransmissionAngle(){

  // c_of_z = 1500*(1+epsilon*(z-1+e^(-z)));
  // c_of_0 = 1500*(1+epsilon*(z-1*e^-z))));
  // zc = water_depth; // or sound speed gradient? 
  // z = (2*(z-zc))/zc;

  c_of_z = sound_speed_gradient*z;
  c_of_0 = surface_sound_speed;

  
  for (theta = 0; theta <= 360; theta ++){
    R = c_of_z/(g*cosd(theta));

    s = R*(NAV_HEADING-theta);

    r(s) = R*(sind(theta_knot)+sind(s/(R - theta_knot)));
    z(s) = R*cosd(s/R-theta_knot)-c(0)/g;

    if (r(s) == sqrt((NAV_X-collabNAV_X)^2 + (NAV_Y - collabNAV_Y)^2) && z(S)==collaDEPTH) {
      return true;
    }

    else {
      return false;
    }
  }
};



    
