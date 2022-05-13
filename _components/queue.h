#include <soc/soc.h> 
#include <soc/rtc_cntl_reg.h>
#include <stdio.h>
#include <stdlib.h>

//这是队列数据结构体，数据是可以以结构体存在的
typedef struct{
  int sender;
  char *msg;
}Data;

//以下句柄名字以x开头，用作双核版本的队列测试
xQueueHandle xqueue0;  //创建的测试队列句柄，我们定义数据为int型
xQueueHandle xqueue1;  //创建的测试队列句柄，我们定义数据为字符串型
xQueueHandle xqueue2;  //创建的测试队列句柄，我们定义数据为结构体型，结构体里面包括一个int型以及一个字符串型
TaskHandle_t xTask0;   //任务0的句柄
TaskHandle_t xTask1;   //任务1的句柄

void Task1(void *pvParameters) {
  //在这里可以添加一些代码，这样的话这个任务执行时会先执行一次这里的内容（当然后面进入while循环之后不会再执行这部分了）
  Data data_send;  //创建一个数据结构体用于发送数据
  data_send.sender = 1314;  //这个数据结构体的整数直接赋值为1314
  BaseType_t xStatus;  //用于状态返回在下面会用到
  BaseType_t xStatus1;
  BaseType_t xStatus2;
  int send_int = 615;
  char send_str[10] = "cx??";
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);   // 阻止任务的时间，直到队列有空闲空间 ，应该是如果发送需要阻滞等待（比如队列满了）或者别的情况需要用到的
  while(1)
  {
    vTaskDelay(1200);
    Serial.print("PRO_CPU正在运行：");
    Serial.println(xPortGetCoreID());
    Serial.print("发送任务固定在： ");
    Serial.println(xTaskGetAffinity(xTask0));    //获取任务被固定到哪里，xTask1 就是 Task0任务本身的句柄
    data_send.msg = (char *)malloc(20);  //分配所需的内存空间，并返回一个指向它的指针，里面传入的参数是SIZE。
    memset(data_send.msg, 0, 20);  //清空这个data_send，也就是上面分配的这个空间
    // 从存储区 str2 复制 n 个字节到存储区 str1。 str2就是"hello world" ，str1就是data_send.msg ， strlen("hello world")就是n
    memcpy(data_send.msg, "hello world", strlen("hello world"));  
    xStatus = xQueueSendToFront( xqueue2, &data_send, xTicksToWait );  //发送data_send这个数据结构体到 xqueue2 队列
    
    xStatus1 = xQueueSendToFront( xqueue0, &send_int, xTicksToWait );  //发送send_int这个数据结构体到 xqueue0 队列
    xStatus2 = xQueueSendToFront( xqueue1, &send_str, xTicksToWait );  //发送send_str这个数据结构体到 xqueue1 队列
    if( xStatus == pdPASS && xStatus1 == pdPASS && xStatus2 == pdPASS) {
      Serial.println("send data OK");  // 发送正常 
    }
    Serial.println("******************************************************************");
  }
}
 
void Task2(void *pvParameters) {
  //  这些变量作用与上面相同，只是这里的data_get就是我们从队列里面获取的东西了
  BaseType_t xStatus;
  BaseType_t xStatus1;
  BaseType_t xStatus2;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(50);  //这里就是用于取数据阻塞了，我觉得本来这种收发就不可能同步，但是应该接收来满足发送，接收速度大于发送速度才行
  Data data_get;
  int get_int;
  char get_str[10];
  while(1)
  {
    vTaskDelay(800);
    Serial.print("APP_CPU正在运行：");
    Serial.println(xPortGetCoreID());
    Serial.print("获取任务固定在：");
    Serial.println(xTaskGetAffinity(xTask1));
    xStatus = xQueueReceive( xqueue2, &data_get, xTicksToWait );  //从队列2中取一条数据
    xStatus = xQueueReceive( xqueue0, &get_int, xTicksToWait );
    xStatus = xQueueReceive( xqueue1, &get_str, xTicksToWait );
    if(xStatus == pdPASS){
      free(data_get.msg);  //释放数据结构体的字符串部分的空间
      Serial.print("获取结构体数据整数部分：");
      Serial.println(data_get.sender);
      Serial.print("获取结构体数据字符串部分：");
      Serial.println(data_get.msg);
      Serial.print("队列零的整数获取：");
      Serial.println(get_int);
      Serial.print("队列1的字符串获取：");
      Serial.println(get_str);
    }
    Serial.println("******************************************************************");
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);//关闭低电压检测,避免无限重启
  Serial.begin(112500);
  delay(1000);
  Serial.print("获取Setup的优先级： ");
  Serial.println(uxTaskPriorityGet(NULL));  //优先级获取函数
  
  xqueue0 = xQueueCreate( 10, sizeof( int ) );
  xqueue1 = xQueueCreate( 10, sizeof( char[10] ) );
  xqueue2 = xQueueCreate(10, sizeof(Data));
  
  xTaskCreatePinnedToCore(Task1, "Task1", 10000, NULL, 11, &xTask0,  0);  //最后一个参数至关重要，决定这个任务创建在哪个核上.PRO_CPU 为 0, APP_CPU 为 1,或者 tskNO_AFFINITY 允许任务在两者上运行.
  xTaskCreatePinnedToCore(Task2, "Task2", 10000, NULL, 12, &xTask1,  1);  //xTaskGetAffinity(xTask1)  是可以查询到任务被固定到哪里的
  Serial.print("获取xTask0的优先级： ");
  Serial.println(uxTaskPriorityGet(xTask0));  //优先级获取函数
  Serial.print("获取xTask1的优先级： ");
  Serial.println(uxTaskPriorityGet(xTask1));  //优先级获取函数
  if(xqueue0 != NULL && xqueue1 != NULL && xqueue2 != NULL) Serial.println("开始队列测试!!!");
  Serial.println("******************************************************************");
}
 
void loop() {
}

