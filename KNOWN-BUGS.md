# Severity explained:

level 1: Balance issues or minor visual issues.

level 2: A nuisance for the user, either from a technical perspective or mechanical one.
         These bugs are a result of big semantic mistakes in the code.

level 3: Game breaking in nature. Will probably have the same origin as a level 2 bug.


###### Bug template:
- Description: *Mandatory
- Possible solution: *Optional
- Cause: *Optional *guessing is allowed


## LEVEL 3:
- Description: crashing on higher res height maps
- Possible solution: converting to 64-bit and/or not storing the data on the cpu longer than needed and rather sent it to a vbo. Only caring about every other pixel (although not really a solution but rather a mitigation)-
- Cause: solution is 32-bit and therefore the block size is not large enough to hold all the data  

## LEVEL 2:
- Description: Going from camera::state::LOCK_TO_TARGET to any other state
				makes camera go back to previous rotation
- Possible solution:
- Cause: pitch and yaw stays the same while in camera::state::LOCK_TO_TARGET

- Description: blending makes parts of the plane invisible
- Possible solution:
- Cause: Not drawing all transparent last and not caring about distance from camera.

## LEVEL 1:

- Description: contour lines are maybe too "aggressive" or "dominant" on flat surfaces.
- Possible solution:
- Cause: contour have no real size, only a height.
