#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot
void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    // Call the service and pass the reference speeds
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot.");
}


void process_image_callback(const sensor_msgs::Image& img) {
    // Define white pixel threshold
    const int white_pixel = 255;

    // Variables to track ball position and motion commands
    bool ball_found = false;
    float linear_x = 0.0;
    float angular_z = 0.0;

    // Iterate through each pixel in the image
    for (int i = 0; i < img.height; i++) {
        for (int j = 0; j < img.step; j += 3) { // Step by 3 to check RGB
            if (img.data[i * img.step + j] == white_pixel &&
                img.data[i * img.step + j + 1] == white_pixel &&
                img.data[i * img.step + j + 2] == white_pixel) {
                
                // Implement ball detected logic here like which direction to move
                ball_found = true;
                linear_x = 0.5;
                angular_z = (img.width*3/2 - j) * (2. / (img.width*3/2));
                break; // Exit loop because we found the ball, no more processing is needed
            }
        }
        if (ball_found) break; // Break the outer loop
    }

   // Handle the motion here or not, as per your decision
    drive_robot(linear_x, angular_z);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;
    
    // Define a client service to request command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
    
    // Subscribe to /camera/rgb/image_raw topic
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);
    
    // Handke ROS communication events
    ros::spin();
    
    return 0;
}
