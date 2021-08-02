#include <jni.h>
#include <string>
#include <android/log.h>


//#define LOG_TAG _CTagHead(__FILE__,__func__)
#define LOG_TAG "C_TAG"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ndkdemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkdemo_MainActivity_ginsengTest(JNIEnv *env, jclass clazz, jshort s, jint i,
                                                  jlong l, jfloat f, jdouble d, jchar c, jboolean z,
                                                  jbyte b, jstring str, jobject obj, jobject p,
                                                  jintArray arr) {
    // TODO: implement ginsengTest()
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ndkdemo_MainActivity_operateString(JNIEnv *env, jobject thiz, jstring str) {
    //从java的内存中把字符串拷贝出来  在native使用
    const char *strFromJava = (char *) env->GetStringUTFChars(str, NULL);
    if (strFromJava == NULL) {
        //必须空检查
        return NULL;
    }

    //将strFromJava拷贝到buff中,待会儿好拿去生成字符串
    char buff[128] = {0};
    strcpy(buff, strFromJava);
    strcat(buff, " 在字符串后面加点东西");

    //释放资源
    env->ReleaseStringUTFChars(str, strFromJava);

    //自动转为Unicode
    return env->NewStringUTF(buff);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_ndkdemo_MainActivity_sumArray(JNIEnv *env, jobject thiz, jintArray array) {
    // TODO: implement sumArray()
    //数组求和
    int result = 0;

    //方式1  推荐使用
    jint arr_len = env->GetArrayLength(array);
    //动态申请数组
    jint *c_array = (jint *) malloc(arr_len * sizeof(jint));
    //初始化数组元素内容为0
    memset(c_array, 0, sizeof(jint) * arr_len);
    //将java数组的[0-arr_len)位置的元素拷贝到c_array数组中
    env->GetIntArrayRegion(array, 0, arr_len, c_array);
    for (int i = 0; i < arr_len; ++i) {
        result += c_array[i];
    }
    //动态申请的内存 必须释放
    free(c_array);

    return result;
}extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_example_ndkdemo_MainActivity_init2DArray(JNIEnv *env, jobject thiz, jint size) {

    //创建一个size*size大小的二维数组

    //jobjectArray是用来装对象数组的   Java数组就是一个对象 int[]
    jclass classIntArray = env->FindClass("[I");
    if (classIntArray == NULL) {
        return NULL;
    }
    //创建一个数组对象,元素为classIntArray
    jobjectArray result = env->NewObjectArray(size, classIntArray, NULL);
    if (result == NULL) {
        return NULL;
    }
    for (int i = 0; i < size; ++i) {
        jint buff[100];
        //创建第二维的数组 是第一维数组的一个元素
        jintArray intArr = env->NewIntArray(size);
        if (intArr == NULL) {
            return NULL;
        }
        for (int j = 0; j < size; ++j) {
            //这里随便设置一个值
            buff[j] = 666;
        }
        //给一个jintArray设置数据
        env->SetIntArrayRegion(intArr, 0, size, buff);
        //给一个jobjectArray设置数据 第i索引,数据位intArr
        env->SetObjectArrayElement(result, i, intArr);
        //及时移除引用
        env->DeleteLocalRef(intArr);
    }

    return result;


}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ndkdemo_MainActivity_callStaticMethod(JNIEnv *env, jobject thiz, jstring str) {
    //调用某个类的static方法
    //1. 从classpath路径下搜索MyJNIClass这个类,并返回该类的Class对象
    jclass clazz = env->FindClass("com/example/ndkdemo/MyJNIClass");
    //2. 从clazz类中查找getDes方法 得到这个静态方法的方法id
    jmethodID mid_get_des = env->GetStaticMethodID(clazz, "getDes", "(Ljava/lang/String;)Ljava/lang/String;");
    //3. 构建入参,调用static方法,获取返回值
    jstring str_arg = env->NewStringUTF("我是 c++，我要调用 java static method");
    jstring result = (jstring) env->CallStaticObjectMethod(clazz, mid_get_des, str_arg);
    const char *result_str = env->GetStringUTFChars(result, NULL);
    LOGI("获取到Java层返回的数据 : %s", result_str);

    //4. 移除局部引用
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(str_arg);
    env->DeleteLocalRef(result);

    return env->NewStringUTF(result_str);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkdemo_MainActivity_callAndCreateIntanceMethod(JNIEnv *env, jobject thiz) {

    jclass clazz = env->FindClass("com/example/ndkdemo/MyJNIClass");
    //获取构造方法的方法id
    jmethodID mid_construct = env->GetMethodID(clazz, "<init>", "()V");
    //获取getAge方法的方法id
    jmethodID mid_get_age = env->GetMethodID(clazz, "getAge", "()I");
    jmethodID mid_set_age = env->GetMethodID(clazz, "setAge", "(I)V");
    jobject jobj = env->NewObject(clazz, mid_construct);

    //调用方法setAge
    env->CallVoidMethod(jobj, mid_set_age, 20);
    //再调用方法getAge 获取返回值 打印输出
    jint age = env->CallIntMethod(jobj, mid_get_age);
    LOGI("获取到 age = %d", age);

    //凡是使用是jobject的子类,都需要移除引用
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(jobj);

}


void willCrash() {
    JNIEnv *env = NULL;
    int version = env->GetVersion();
}



extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkdemo_MainActivity_testNativeCrash(JNIEnv *env, jobject thiz) {
    LOGI("崩溃前");
    willCrash();
    //后面的代码是执行不到的,因为崩溃了
    LOGI("崩溃后");
    printf("oooo");}