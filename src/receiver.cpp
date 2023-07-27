#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32_multi_array.hpp"
#include "sensor_msgs/msg/imu.hpp"

#include "BarelangFC-Receiver.h"

bool UDP_LAN = false;

using namespace BarelangFC;

void BarelangReceiver::die(char *s)
{
    perror(s);
    exit(1);
}

int abs(int data)
{
    if (data < 0)
        data = data * -1;
    else
        data = data;
    return data;
}

void BarelangReceiver::initialize_()
{
    if (UDP_LAN)
    {
        socTrim = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        memset((char *)&addrTerima, 0, sizeof(addrTerima));
        addrTerima.sin_family = AF_INET;
        addrTerima.sin_port = htons(PORT_);
        addrTerima.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(socTrim, (struct sockaddr *)&addrTerima, sizeof(addrTerima));
    }
    else
    {
        if ((v = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        {
            die("socket");
        }
        memset((char *)&si_meV, 0, sizeof(si_meV));
        si_meV.sin_family = AF_INET;
        si_meV.sin_port = htons(PORT_);
        si_meV.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(v, (struct sockaddr *)&si_meV, sizeof(si_meV)) == -1)
        {
            die("bind");
        }
    }
}

rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr Volt_n_Odometry;
rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr Imu;
int hiCurrent = 0;
void BarelangReceiver::processing_()
{

    fflush(stdout);
    bzero(recv, sizeof(recv));
    if (UDP_LAN)
    {
        recvfrom(socTrim, recv, BUFLEN_, 0, (struct sockaddr *)&addrKirim, &slen);
    }
    else
    {
        if ((recv_lenV = recvfrom(v, recv, BUFLEN_, 0, (struct sockaddr *)&si_otherV, &slenV)) == -1)
        {
            die("recvfrom()");
        }
    }
    // printf("Data  : %s\n",recv);
    index_ = 0;
    parse = strtok(recv, ";");

    while (parse != NULL)
    {
        index_++;
        if (index_ == 1)
        {
            sscanf(parse, "%d", &roll);
        }
        else if (index_ == 2)
        {
            sscanf(parse, "%d", &pitch);
        }
        else if (index_ == 3)
        {
            sscanf(parse, "%d", &yaw);
        }
        else if (index_ == 4)
        {
            sscanf(parse, "%d", &voltage);
        }
        else if (index_ == 5)
        {
            sscanf(parse, "%d", &vx);
        }
        else if (index_ == 6)
        {
            sscanf(parse, "%d", &vy);
        }
        else if (index_ == 7)
        {
            sscanf(parse, "%d", &va);
        }
        else if (index_ == 8)
        {
            sscanf(parse, "%d", &walkActive);
        }
        else if (index_ == 9)
        {
            sscanf(parse, "%d", &supportLeg);
        }
        else if (index_ == 10)
        {
            sscanf(parse, "%d", &lKneeCurr);
        }
        else if (index_ == 11)
        {
            sscanf(parse, "%d", &rKneeCurr);
        }
        parse = strtok(NULL, ";");
    }
    // if (lKneeCurr > hiCurrent)
    // {
    //     hiCurrent = lKneeCurr;
    // }
    // printf("%d\n", hiCurrent);

    auto msg_vno = std_msgs::msg::Int32MultiArray();
    msg_vno.data = {voltage, walkActive, supportLeg, lKneeCurr, vx, vy, va};
    Volt_n_Odometry->publish(msg_vno);

    auto msg_imu = sensor_msgs::msg::Imu();
    msg_imu.angular_velocity.x = roll;
    msg_imu.angular_velocity.y = pitch;
    msg_imu.angular_velocity.z = yaw;
    Imu->publish(msg_imu);
}


rclcpp::TimerBase::SharedPtr timer_;

int main(int argc, char const *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("receiver_node");
    Imu = node->create_publisher<sensor_msgs::msg::Imu>("imu", 1);
    Volt_n_Odometry = node->create_publisher<std_msgs::msg::Int32MultiArray>("voltodom", 1);
    BarelangReceiver receiver;
    receiver.initialize_();
    timer_ = node->create_wall_timer(
        std::chrono::microseconds(30000),
        std::bind(&BarelangReceiver::processing_, receiver));
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}
