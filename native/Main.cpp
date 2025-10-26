#include "jni.h"
#include <iostream>
#include <vector>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#pragma comment(lib, "runtimeobject.lib")
#pragma comment(lib, "windowsapp.lib")

using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

#ifdef __cplusplus
extern "C" {
#endif
    JNIEXPORT jobjectArray JNICALL Java_WindowsMediaPlayerController_nativeGetMediaSessions
    (JNIEnv* env, jobject obj) {

        try {
            jclass mediaSessionClass = env->FindClass("WindowsMediaPlayerController$MediaSession");
            if (mediaSessionClass == nullptr) {
                return nullptr;
            }

            jmethodID mediaSessionConstructor = env->GetMethodID(
                mediaSessionClass,
                "<init>",
                "(LWindowsMediaPlayerController$MediaInfo;Ljava/lang/String;I)V"
            );

            jclass mediaInfoClass = env->FindClass("WindowsMediaPlayerController$MediaInfo");
            if (mediaInfoClass == nullptr) {
                return nullptr;
            }

            jmethodID mediaInfoConstructor = env->GetMethodID(
                mediaInfoClass,
                "<init>",
                "(Ljava/lang/String;Ljava/lang/String;[BJJZ)V"
            );

            auto smtc = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto sessions = smtc.GetSessions();

            int sessionCount = sessions.Size();
            jobjectArray result = env->NewObjectArray(
                sessionCount,
                mediaSessionClass,
                nullptr
            );

            for (int i = 0; i < sessionCount; ++i) {
                auto session = sessions.GetAt(i);
                auto timeline = session.GetTimelineProperties();
                auto mediaProperties = session.TryGetMediaPropertiesAsync().get();
                jbyteArray jArtwork;
                auto thumbnail = mediaProperties.Thumbnail();
                if (thumbnail) {
                    try {
                        auto thumbnailStream = thumbnail.OpenReadAsync().get();
                        auto reader = DataReader(thumbnailStream.GetInputStreamAt(0));
                        reader.LoadAsync(thumbnailStream.Size()).get();

                        std::vector<uint8_t> buffer(thumbnailStream.Size());
                        auto bufferView = array_view<uint8_t>(buffer);
                        reader.ReadBytes(bufferView);
                        reader.Close();
                        thumbnailStream.Close();

                        jArtwork = env->NewByteArray(static_cast<jsize>(buffer.size()));
                        env->SetByteArrayRegion(
                            jArtwork,
                            0,
                            static_cast<jsize>(buffer.size()),
                            reinterpret_cast<const jbyte*>(buffer.data())
                        );
                    }
                    catch (...) {
                        jArtwork = env->NewByteArray(0);
                    }
                }
                else {
                    jArtwork = env->NewByteArray(0);
                }
                jstring jTitle = env->NewStringUTF(to_string(mediaProperties.Title()).c_str());
                jstring jArtist = env->NewStringUTF(to_string(mediaProperties.Artist()).c_str());
                jboolean jPlaying = session.GetPlaybackInfo().PlaybackStatus() ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                jlong jPosition;
                if (jPlaying) {
                    jPosition = std::chrono::duration_cast<std::chrono::seconds>(
                        winrt::clock::now() - timeline.LastUpdatedTime() + timeline.Position()
                    ).count();
                }
                else {
                    jPosition = std::chrono::duration_cast<std::chrono::seconds>(
                        timeline.Position()
                    ).count();
                }
                jlong jDuration = std::chrono::duration_cast<std::chrono::seconds>(
                    timeline.EndTime() - timeline.StartTime()
                ).count();
                jobject mediaInfo = env->NewObject(
                    mediaInfoClass,
                    mediaInfoConstructor,
                    jTitle,
                    jArtist,
                    jArtwork,
                    jPosition,
                    jDuration,
                    jPlaying
                );

                jstring jOwner = env->NewStringUTF(
                    to_string(session.SourceAppUserModelId()).c_str()
                );

                jobject mediaSession = env->NewObject(
                    mediaSessionClass,
                    mediaSessionConstructor,
                    mediaInfo,
                    jOwner,
                    i
                );

                env->SetObjectArrayElement(result, i, mediaSession);
                env->DeleteLocalRef(jTitle);
                env->DeleteLocalRef(jArtist);
                env->DeleteLocalRef(jArtwork);
                env->DeleteLocalRef(jOwner);
                env->DeleteLocalRef(mediaInfo);
                env->DeleteLocalRef(mediaSession);
            }

            return result;

        }
        catch (const std::exception& e) {
            return nullptr;
        }
        catch (...) {
            return nullptr;
        }
    }

    JNIEXPORT void JNICALL Java_WindowsMediaPlayerController_nativePlay
    (JNIEnv* env, jobject obj, jint sessionIndex) {

        try {
            auto smtc = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto sessions = smtc.GetSessions();

            if (sessionIndex >= 0 && sessionIndex < (jint)sessions.Size()) {
                auto session = sessions.GetAt(sessionIndex);
                session.TryPlayAsync().get();
            }
        }
        catch (const std::exception& e) {
        }
    }

    JNIEXPORT void JNICALL Java_WindowsMediaPlayerController_nativePause
    (JNIEnv* env, jobject obj, jint sessionIndex) {

        try {
            auto smtc = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto sessions = smtc.GetSessions();

            if (sessionIndex >= 0 && sessionIndex < (jint)sessions.Size()) {
                auto session = sessions.GetAt(sessionIndex);
                session.TryPauseAsync().get();
            }
        }
        catch (const std::exception& e) {
        }
    }

    JNIEXPORT void JNICALL Java_WindowsMediaPlayerController_nativePlayPause
    (JNIEnv* env, jobject obj, jint sessionIndex) {

        try {
            auto smtc = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto sessions = smtc.GetSessions();

            if (sessionIndex >= 0 && sessionIndex < (jint)sessions.Size()) {
                auto session = sessions.GetAt(sessionIndex);
                session.TryTogglePlayPauseAsync().get();
            }
        }
        catch (const std::exception& e) {
        }
    }

    JNIEXPORT void JNICALL Java_WindowsMediaPlayerController_nativeStop
    (JNIEnv* env, jobject obj, jint sessionIndex) {

        try {
            auto smtc = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto sessions = smtc.GetSessions();

            if (sessionIndex >= 0 && sessionIndex < (jint)sessions.Size()) {
                auto session = sessions.GetAt(sessionIndex);
                session.TryStopAsync().get();
            }
        }
        catch (const std::exception& e) {
        }
    }

    JNIEXPORT void JNICALL Java_WindowsMediaPlayerController_nativeNext
    (JNIEnv* env, jobject obj, jint sessionIndex) {

        try {
            auto smtc = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto sessions = smtc.GetSessions();

            if (sessionIndex >= 0 && sessionIndex < (jint)sessions.Size()) {
                auto session = sessions.GetAt(sessionIndex);
                session.TrySkipNextAsync().get();
            }
        }
        catch (const std::exception& e) {
        }
    }

    JNIEXPORT void JNICALL Java_WindowsMediaPlayerController_nativePrevious
    (JNIEnv* env, jobject obj, jint sessionIndex) {

        try {
            auto smtc = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto sessions = smtc.GetSessions();

            if (sessionIndex >= 0 && sessionIndex < (jint)sessions.Size()) {
                auto session = sessions.GetAt(sessionIndex);
                session.TrySkipPreviousAsync().get();
            }
        }
        catch (const std::exception& e) {
        }
    }

#ifdef __cplusplus
}
#endif