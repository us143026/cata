#include "catanav.h"
#include "os5000/CompassData.h"
#define PI 3.14159
#define NUM_OF_WAYPOINTS 3
#define STATE_NAVIGATING 1
#define STATE_IDLE 0
#define STATE_INIT -1

using namespace gps_common;

bool gps_received = false;
bool compass_received = false;
bool odometry_received = false;
bool laser_received = false;
float compassThreshold = 20.0f;
//float turningSpeed = 0.25f;

bool orientateRobot2(float yaw);
float trackHeading(float target_heading, float forward_speed);
double getHeadingDifference(float target_heading, float current_heading);
float deg2rad(float degrees)
{
  return degrees/180.0*PI;
}
float rad2deg(float radians)
{
  return radians/PI*180.0;
}

	struct lat_and_long { double latitude; double longitude;};
	
	struct lat_and_long current_position;
	struct lat_and_long nav_waypoints[NUM_OF_WAYPOINTS];
  std::vector<float> laser_scan_ranges;
  nav_msgs::Odometry current_odom;

std::vector<float> downsampleLaserScan(int window)
{
  std::vector<float> newScan;
  int window_counter=0;
  float min_range=999999.9;
  for(int index=0; index<laser_scan_ranges.size(); index++)
  {
       if(window_counter<window)
       {
          if(laser_scan_ranges[index]<min_range)
              min_range = laser_scan_ranges[index];
             
          window_counter++;
       }
       else
       {
          newScan.push_back(min_range);
          min_range=999999;
          window_counter=0;
   
       }
          
  }
  return newScan;
}
std::vector<int> thresholdLaserScan(std::vector<float> scan, float min_distance)
{
  std::vector<int> output_scan;

  for(int index=0; index<scan.size(); index++)
  {
      if(scan[index]<min_distance)
          output_scan.push_back(1);
      else
          output_scan.push_back(0);
  }
  return output_scan;
}
void printLaserScan(std::vector<float> scan)
{
    for(int index=0; index<scan.size(); index++)
        printf("%f ", scan[index]);
    printf("\n");
}
void printBinaryLaserScan(std::vector<int> scan)
{
    for(int index=0; index<scan.size(); index++)
        printf("%d ", scan[index]);
    printf("\n");
}
void turnNumberOfDegrees(float angle)
{
   float w = current_odom.pose.pose.orientation.w;
   float z = current_odom.pose.pose.orientation.z;
   float initialHeading = atan2(w, z);
   float turning_speed = 0.25;
   float heading_diff = getHeadingDifference(initialHeading, currentYaw);
   
   if(angle>0)
   {
      while(heading_diff<angle)   
        heading_diff = getHeadingDifference(initialHeading, currentYaw);
        pubCmdVelocity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, turning_speed);
        ros::spinOnce();
   }
   else
   {
      while(heading_diff>angle)
        heading_diff = getHeadingDifference(initialHeading, currentYaw);
        pubCmdVelocity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -turning_speed);
        ros::spinOnce();
      
   }
   

    
    
}
void goForwardNumberOfMeters(float distance, float forward_speed)
{
    float x0 = current_odom.pose.pose.position.x;
    float y0 = current_odom.pose.pose.position.y;
    
    float dist = 0;
    while(dist<distance)
    {
        float x=  current_odom.pose.pose.position.x;
        float y = current_odom.pose.pose.position.y;
        dist = sqrt( (x-x0)*(x-x0)+(y-y0)*(y-y0) );
        pubCmdVelocity(forward_speed, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        ros::spinOnce();
    }
    pubCmdVelocity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
 
}
double getHeadingDifference(float target_heading, float current_heading)
{
	float d = deg2rad(target_heading-current_heading);

	float diff_sign = asin(sin(d));
	float diff;
	if(diff_sign>0)
    diff =  acos(cos(d));
  else
    diff = -acos(cos(d));
  
  return(rad2deg(diff));  
}

double getHeadingToWaypoint(struct lat_and_long target_position)
{
  struct lat_and_long UTM_target_position;
  struct lat_and_long UTM_current_position;
  std::string zone;
  gps_common::LLtoUTM(target_position.latitude, target_position.longitude, UTM_target_position.latitude, UTM_target_position.longitude, zone);
  gps_common::LLtoUTM(current_position.latitude, current_position.longitude, UTM_current_position.latitude, UTM_current_position.longitude, zone);
  
  double dy = UTM_target_position.latitude-UTM_current_position.latitude;
  double dx = UTM_target_position.longitude-UTM_current_position.longitude;
  
//  double dy = target_position.latitude-current_position.latitude;
//  double dx = target_position.longitude-current_position.longitude;
  
  double heading_to_waypoint = rad2deg(atan2(dx, dy));
  
 //printf("target UTM\n");
  //printf("Lat: %f   Lon: %f\n\n", UTM_target_position.latitude, UTM_target_position.longitude);
  
  //printf("current UTM\n");
 // printf("Lat: %f   Lon: %f\n\n", UTM_current_position.latitude, UTM_current_position.longitude);
  
 // printf("heading to waypoint: %f\n", rad2deg(heading_to_waypoint));
  return heading_to_waypoint;
  
}
double getDistanceToWaypoint(struct lat_and_long target_position)
{
  struct lat_and_long UTM_target_position;
  struct lat_and_long UTM_current_position;
  std::string zone;
  gps_common::LLtoUTM(target_position.latitude, target_position.longitude, UTM_target_position.latitude, UTM_target_position.longitude, zone);
  gps_common::LLtoUTM(current_position.latitude, current_position.longitude, UTM_current_position.latitude, UTM_current_position.longitude, zone);
  
  double dy = UTM_target_position.latitude -UTM_current_position.latitude;
  double dx = UTM_target_position.longitude-UTM_current_position.longitude;
  double distance_to_waypoint = sqrt(dx*dx+dy*dy);
  
  printf("distance to waypoint: %f\n", distance_to_waypoint);
  return distance_to_waypoint;
  
}
int main(int argc, char **argv)
{
	nav_waypoints[0].latitude = 42.678980; nav_waypoints[0].longitude = -83.196129;
	nav_waypoints[1].latitude = 42.679022; nav_waypoints[1].longitude = -83.196122;
	nav_waypoints[2].latitude = 42.678989; nav_waypoints[2].longitude = -83.196159;
	
	//if(system("clear")==true){}
	
	//printf("CCNY Robotics Lab\nCata Test Navigation\n\n");
	
	ros::init(argc, argv, "catanav");
	ros::NodeHandle n;
	
	ros::Subscriber subscriberGPS = n.subscribe("/fix", 100, gpsSubscriberCallBack);
	ros::Subscriber subscriberCompass = n.subscribe("/compassData", 1, compassSubscriberCallBack);
	ros::Subscriber subscriberTarget = n.subscribe("/catanav/target", 1, targetSubscriberCallBack);
	ros::Subscriber subscriberOdom = n.subscribe("/base_odom", 1, odomSubscriberCallBack);
	ros::Subscriber subscriberLaser = n.subscribe("/scan", 1, laserSubscriberCallBack);

	publisherStatus = n.advertise<std_msgs::String>("/catanav/status", 1);
	publisherCmdVel = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
	
	ros::Rate loop_rate(1);
	int state = STATE_INIT;
	while(ros::ok())
	{
		// let ROS spin once so we can get some initial data from sensors
		ros::spinOnce();
		loop_rate.sleep();
    
		  // navigation
		
	  float distance_threshold = 1;
	  float forward_speed = 0.5;
	  
	  //pubCmdVelocity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5);
	   
	  printf("gps: %d\n", gps_received);
	  printf("compass: %d\n", compass_received);
	  printf("odometry: %d\n", odometry_received);
	  printf("laser: %d\n", laser_received);
	  
	  printf("state: %d\n", state);
	  
	  if(state==STATE_NAVIGATING)
	  {
	      if( !gps_received || !compass_received || !odometry_received || !laser_received )
	          state = STATE_IDLE;
	  }
	  else if (state == STATE_INIT)
	  {
	    	if( gps_received && compass_received && odometry_received && laser_received )
	    	{
	          state = STATE_NAVIGATING;
	      }
	  }

	  if(state==STATE_NAVIGATING)
	  {
	      // int waypoint_index = 0;
	      //  for(int waypoint_index=0; waypoint_index<1; waypoint_index++)
	       
	        for(int waypoint_index=0; waypoint_index<NUM_OF_WAYPOINTS; waypoint_index++)
	        {
	          struct lat_and_long nextWaypoint = nav_waypoints[waypoint_index];
	         
	          orientateRobot2(getHeadingToWaypoint(nextWaypoint));
	
	          float distanceToNextWaypoint = getDistanceToWaypoint(nextWaypoint);
	        
	          
	          
	          float rotation_rate;
	          while(distanceToNextWaypoint>distance_threshold)
	          {
	               float headingToNextWaypoint = getHeadingToWaypoint(nextWaypoint);
	               rotation_rate = trackHeading(headingToNextWaypoint, forward_speed);
	               distanceToNextWaypoint = getDistanceToWaypoint(nextWaypoint);
	               pubCmdVelocity(forward_speed, 0.0f, 0.0f, 0.0f, 0.0f,   rotation_rate); 
	               	    if(system("clear")==true){}
	                    printf("Cata - CCNY Robotics 2011\n\n");
	                    printf("current position(lat:%f north, lon:%f east\n", current_position.latitude, current_position.longitude);
	                    printf("current orientation: %f\n\n", currentYaw);
	                    printf("next waypoint(lat:%f north, lon:%f east\n", nextWaypoint.latitude, nextWaypoint.longitude);
	                    printf("distance to next waypoint: %f\n", distanceToNextWaypoint);
	                    printf("heading to next waypoint: %f\n", headingToNextWaypoint);
	                    printf("command rotation vel: %f\n", rotation_rate);
	                    ros::spinOnce();
	                    loop_rate.sleep();
	          }
	        }
	        state = STATE_IDLE;
	  }
	  else
	  {
	        pubCmdVelocity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	        printf("DONE!\n\n");
	  }
	  /*
	  if(system("clear")==true){}
	  std::vector<float> scan = downsampleLaserScan(40);
	  std::vector<int> binary_scan = thresholdLaserScan(scan, 1.5);
	  printLaserScan(scan);
	  printf("\n");
	  printBinaryLaserScan(binary_scan);
	  
	 // turnNumberOfDegrees(10);
    turnNumberOfDegrees(10);
*/
	 
	}

	return 0;
}

float trackHeading(float target_heading, float forward_speed)
{
   float heading_error = getHeadingDifference(target_heading, currentYaw);
   float gain = -0.01;
   float turning_speed = gain*heading_error;
   if(turning_speed > 0.5)
      turning_speed = 0.5;
   if(turning_speed < -0.5)
      turning_speed = -0.5;
  
   return turning_speed;
}

bool orientateRobot2(float yaw)
{
	float targetYaw = yaw;
	ros::Rate loop_rate(10);
  float signedDiff = getHeadingDifference(targetYaw, currentYaw);
  float initialSignedDiff = signedDiff;
  while( abs(signedDiff)>compassThreshold )
  {
    if(initialSignedDiff > 0)
      pubCmdVelocity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5); // go left
    else
      pubCmdVelocity(0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.5); // go right
  
    signedDiff = getHeadingDifference(targetYaw, currentYaw);
    if(system("clear")==true){}
    printf("current yaw: %f\n", currentYaw);
    printf("target yaw: %f\n", targetYaw);
    printf("angle diff: %f\n", signedDiff);
    
    ros::spinOnce();
	  loop_rate.sleep();
  }
  return true;
}

//void gpsSubscriberCallBack(const gps_common::GPSFix::ConstPtr& gpsData)
void gpsSubscriberCallBack(const sensor_msgs::NavSatFix::ConstPtr& gpsData)
{

	if(gpsData->latitude == gpsData->latitude && gpsData->longitude == gpsData->longitude)
	{
		gpsVariance = 1000*(fabs(rawLatitude - gpsData->latitude) + fabs(rawLongitude - gpsData->longitude));
		rawLatitude =  gpsData->latitude;
		rawLongitude = gpsData->longitude;
		rawAltitude =  gpsData->altitude;
		current_position.latitude = rawLatitude;
		current_position.longitude = rawLongitude;
	}
	else
		printf("invalid gps data\n");
		
	gps_received = true;
}
void compassSubscriberCallBack(const os5000::CompassData::ConstPtr& compassData)
{
	if(compassData->yaw == compassData->yaw)
		currentYaw = compassData->yaw;
	else
		currentYaw = 999.99f;
	compass_received = true;
}

void targetSubscriberCallBack(const std_msgs::String::ConstPtr& msg)
{
	ROS_INFO("updated Target data: %s", msg->data.c_str());
}

void odomSubscriberCallBack(const nav_msgs::Odometry::ConstPtr& odom)
{
  current_odom = *odom;
	//printf("we have odometry (%f, %f, %f)!\n", odom->pose.pose.position.x, odom->pose.pose.position.y, odom->pose.pose.position.z);
//	if(odom->pose.pose.position.x >= 1.0f)
		//printf("goal has been reached\n");
		odometry_received = true;
}
void laserSubscriberCallBack(const sensor_msgs::LaserScan::ConstPtr& scan)
{

	laser_scan_ranges = scan->ranges;
	
	//if(odom->pose.pose.position.x >= 1.0f)
		//printf("goal has been reached\n");
	laser_received = true;
}

bool pubCmdVelocity()
{
	return pubCmdVelocity(0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

bool pubCmdVelocity(float x, float y, float z, float ax, float ay, float az)
{
	//printf("publishing cmd_vel\n");
	geometry_msgs::Twist cmd_vel;
	cmd_vel.linear.x = x;
	cmd_vel.linear.y = y;
	cmd_vel.linear.z = z;  
	cmd_vel.angular.x = ax;
	cmd_vel.angular.y = ay;
	cmd_vel.angular.z = az;
	publisherCmdVel.publish(cmd_vel);
	return true;
}

bool printStatus()
{
	if(system("clear")==true){}
	printf("Cata - CCNY Robotics 2011\n\n");
	printf("robot position(lat:%f north, lon:%f east\n", rawLatitude, rawLongitude);
	printf("robot orientation: yaw %f\n\n", currentYaw);
	
	return true;
}
