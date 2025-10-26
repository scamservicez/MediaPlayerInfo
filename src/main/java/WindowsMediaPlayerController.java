import java.nio.file.Files;
import java.nio.file.Path;

public class WindowsMediaPlayerController {

    private MediaSession[] sessions;
    private int currentSessionIndex = 0;

    static {
        Library("mediaplayer.dll");
    }

    public WindowsMediaPlayerController() {
        refresh();
    }

    private native MediaSession[] nativeGetMediaSessions();
    private native void nativePlay(int sessionIndex);
    private native void nativePause(int sessionIndex);
    private native void nativePlayPause(int sessionIndex);
    private native void nativeStop(int sessionIndex);
    private native void nativeNext(int sessionIndex);
    private native void nativePrevious(int sessionIndex);

    private static void Library(String lib) {
        try (var input = WindowsMediaPlayerController.class.getResourceAsStream("/" + lib)) {
            if (input == null) throw new IllegalStateException("Не найден ресурс " + lib);
            Path temp = Files.createTempFile("mediaplayer_", ".dll");
            Files.copy(input, temp, java.nio.file.StandardCopyOption.REPLACE_EXISTING);
            temp.toFile().deleteOnExit();
            System.load(temp.toAbsolutePath().toString());
        } catch (Exception e) {
            throw new RuntimeException("Ошибка загрузки библиотеки: " + lib, e);
        }
    }

    public void refresh() {
        sessions = nativeGetMediaSessions();
    }

    public MediaSession[] getSessions() {
        return sessions != null ? sessions : new MediaSession[0];
    }

    public MediaSession getCurrentSession() {
        if (sessions != null && currentSessionIndex >= 0 && currentSessionIndex < sessions.length) {
            return sessions[currentSessionIndex];
        }
        return null;
    }

    public void setCurrentSession(int index) {
        if (sessions != null && index >= 0 && index < sessions.length) {
            currentSessionIndex = index;
        }
    }

    public boolean setCurrentSessionByAppName(String appName) {
        if (sessions == null) return false;
        for (int i = 0; i < sessions.length; i++) {
            if (sessions[i].appName().equalsIgnoreCase(appName)) {
                currentSessionIndex = i;
                return true;
            }
        }
        return false;
    }

    public void play() { nativePlay(currentSessionIndex); }
    public void pause() { nativePause(currentSessionIndex); }
    public void playPause() { nativePlayPause(currentSessionIndex); }
    public void stop() { nativeStop(currentSessionIndex); }
    public void next() { nativeNext(currentSessionIndex); }
    public void previous() { nativePrevious(currentSessionIndex); }

    public MediaInfo getCurrentTrackInfo() {
        MediaSession session = getCurrentSession();
        return session != null ? session.mediaInfo() : null;
    }

    public String getPlayState() {
        MediaInfo info = getCurrentTrackInfo();
        return info != null ? (info.playing() ? "Playing" : "Paused") : "Unknown";
    }
}
