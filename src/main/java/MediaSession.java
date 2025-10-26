public record MediaSession(MediaInfo mediaInfo, String appName, int index) {

    @Override
    public String toString() {
        return "MediaSession{" +
                "appName='" + appName + '\'' +
                ", index=" + index +
                ", mediaInfo=" + mediaInfo +
                '}';
    }
}
