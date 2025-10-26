/**
 * @param position сек
 * @param duration сек
 */
public record MediaInfo(String title, String artist, byte[] artwork, long position, long duration, boolean playing) {

    public String getPositionFormatted() {
        return formatTime(position);
    }

    public String getDurationFormatted() {
        return formatTime(duration);
    }

    private String formatTime(long seconds) {
        long h = seconds / 3600;
        long m = (seconds % 3600) / 60;
        long s = seconds % 60;
        return h > 0 ? String.format("%d:%02d:%02d", h, m, s) : String.format("%02d:%02d", m, s);
    }

    @Override
    public String toString() {
        return String.format("Трек: %s\nИсполнитель: %s\nПозиция: %s / %s\nСтатус: %s\nОбложка: %s",
                title, artist, getPositionFormatted(), getDurationFormatted(),
                playing ? "Играет" : "На паузе",
                artwork != null && artwork.length > 0 ? artwork.length + " байт" : "Нет");
    }
}
