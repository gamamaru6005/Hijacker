#include <jni.h>
#include <string>
#include <stdlib.h>
#include <android/log.h>

void logd(char* str){
    __android_log_write(ANDROID_LOG_INFO, "CPP", str);
}

extern "C" jint Java_com_hijacker_MainActivity_checkwpa(JNIEnv* env, jobject obj, jstring str) {
    char *buffer = (char *) malloc(512);
    const char *nativeString = env->GetStringUTFChars(str, 0);
    strcpy(buffer, nativeString);
    env->ReleaseStringUTFChars(str, nativeString);
    char temp[2];
    if(strlen(buffer)>55) {
        strncpy(temp, buffer + 56, 1);
        if(!(temp[0]=='1' || temp[0]=='2' || temp[0]=='3')) return 0;
        else return 1;
    }else return -1;
}
extern "C" jboolean Java_com_hijacker_MainActivity_aireplay(JNIEnv* env, jobject obj, jstring str) {
    char *buffer = (char *) malloc(512);
    const char *nativeString = env->GetStringUTFChars(str, 0);
    strcpy(buffer, nativeString);
    env->ReleaseStringUTFChars(str, nativeString);
    char temp[8];
    strncpy(temp, buffer+10, 7);
    temp[7]='\0';
    return (jboolean)!strcmp(temp, "Sending");
}

extern "C" jint Java_com_hijacker_MainActivity_ps(JNIEnv* env, jobject obj, jstring str) {
    char *buffer = (char *) malloc(512);
    const char *nativeString = env->GetStringUTFChars(str, 0);
    strcpy(buffer, nativeString);
    env->ReleaseStringUTFChars(str, nativeString);
    char pid[7];          //pid text

    strncpy(pid, buffer+10, 6);
    pid[6] = '\0';
    return atoi(pid);
    /*char app[5];          //buffer for reading the app
    strncpy(app, buffer+57, 4);
    app[4] = '\0';
    if(pr==0){
        //airodump
        if (!strcmp(app, "airo")){
            strncpy(pid, buffer+10, 6);
            pid[6] = '\0';
            return atoi(pid);
        }else return 0;
    }else if (pr==1){
        //aireplay
        if (!strcmp(app, "aire")){
            strncpy(pid, buffer+10, 6);
            pid[6] = '\0';
            return atoi(pid);
        }else return 0;
    }else{
        //mdk
        if (!strcmp(app, "mdk3")){
            strncpy(pid, buffer+10, 6);
            pid[6] = '\0';
            return atoi(pid);
        }else return 0;
    }*/
}

extern "C" jint Java_com_hijacker_MainActivity_main(JNIEnv* env, jobject obj, jstring str, jint off){
    int i, j;
    char *buffer = (char*)malloc(512);
    const char *nativeString = env->GetStringUTFChars(str, 0);
    strcpy(buffer, nativeString);
    buffer[env->GetStringLength(str)]='\0';
    i=strlen(buffer)-1;
    while(buffer[i]==' ') i--;
    buffer[i+1]='\0';

    //__android_log_print(ANDROID_LOG_INFO, "CPP", "NULL @ [%d], buffer length=%d", env->GetStringLength(str), strlen(buffer));

    env->ReleaseStringUTFChars(str, nativeString);

    jclass jclass1 = env->FindClass("com/hijacker/MainActivity");
    jmethodID method_ap = env->GetStaticMethodID(jclass1, "addAP", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIIII)V");
    jmethodID method_st = env->GetStaticMethodID(jclass1, "addST", "(Ljava/lang/String;Ljava/lang/String;III)V");

    if( buffer[3]==':' || buffer[3]=='o' ){
        //logd("Found ':' or 'o' @ 3");
        while(buffer[strlen(buffer)-1]=='\n'){
            buffer[strlen(buffer)-1] = '\0';
        }
        //Clear spaces
        for(i=123;i<strlen(buffer);i++){
            if(buffer[i]==' ' && buffer[i+1]==' '){
                for(j=i;j<strlen(buffer);j++){
                    buffer[j] = buffer[j+1];
                }
                i--;
            }
        }
        if(buffer[22]==':'){
            //st
            char st_mac[18], bssid[18], pwr_c[5], lost_c[7], frames_c[10];
            int pwr, lost, frames;

            strncpy(st_mac, buffer+20, 17);
            st_mac[17]='\0';

            if(buffer[1]=='(') strcpy(bssid, "na"); //not associated
            else strncpy(bssid, buffer+1, 17);
            bssid[17]='\0';

            strncpy(pwr_c, buffer+37, 5);
            pwr_c[5]='\0';
            pwr = atoi(pwr_c);

            strncpy(lost_c, buffer+52, 6);
            lost_c[6]='\0';
            lost = atoi(lost_c);

            strncpy(frames_c, buffer+58, 9);
            frames_c[9]='\0';
            frames = atoi(frames_c);

            jstring s1 = env->NewStringUTF(st_mac);
            jstring s2 = env->NewStringUTF(bssid);
            env->CallStaticVoidMethod(jclass1, method_st, s1, s2, pwr, lost, frames);
        }else{
            //ap
            char bssid[18], pwr_c[6], beacons_c[10], data_c[10], ivs_c[6], ch_c[3], enc[5], cipher[5], auth[5], essid[50];
            int pwr, beacons, data, ivs, ch;

            strncpy(bssid, buffer+1, 17);
            bssid[17]='\0';

            strncpy(pwr_c, buffer+18, 5);
            pwr_c[5]='\0';
            pwr = atoi(pwr_c);

            //if off is not 0 then airodump-ng is running for a specific channel
            //so we need to bypass 4 characters after pwr to get the correct results because there is one extra column
            if(off!=0) buffer += 4;

            strncpy(beacons_c, buffer+23, 9);
            beacons_c[9]='\0';
            beacons = atoi(beacons_c);

            strncpy(data_c, buffer+32, 9);
            data_c[9]='\0';
            data = atoi(data_c);

            strncpy(ivs_c, buffer+41, 5);
            ivs_c[5]='\0';
            ivs = atoi(ivs_c);

            strncpy(ch_c, buffer+48, 2);
            ch_c[2]='\0';
            ch = atoi(ch_c);

            strncpy(enc, buffer+57, 4);
            if(enc[3]==' ') enc[3] = '\0';
            else enc[4] = '\0';

            strncpy(cipher, buffer+62, 4);
            cipher[4]='\0';

            strncpy(auth, buffer+69, 4);
            if(auth[3]==' ') auth[3] = '\0';
            else auth[4] = '\0';

            if(buffer[74]!='<'){
                strncpy(essid, buffer+74, 49);
            }else strcpy(essid, "<hidden>\0");
            essid[49]='\0';

            jstring s1 = env->NewStringUTF(essid);
            jstring s2 = env->NewStringUTF(bssid);
            jstring s3 = env->NewStringUTF(enc);
            jstring s4 = env->NewStringUTF(cipher);
            jstring s5 = env->NewStringUTF(auth);
            env->CallStaticVoidMethod(jclass1, method_ap, s1, s2, s3, s4, s5, pwr, beacons, data, ivs, ch);
        }
    }
    return 0;
}