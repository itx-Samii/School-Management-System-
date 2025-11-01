import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.io.*;
import java.util.*;

public class ViewAttendanceForm extends JFrame {

    private JTable table;
    private DefaultTableModel model;

    public ViewAttendanceForm() {
        setTitle("View Attendance Records");
        setSize(600, 400);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setLayout(new BorderLayout());

        JLabel title = new JLabel("Attendance Records", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 18));
        add(title, BorderLayout.NORTH);

        model = new DefaultTableModel();
        model.setColumnIdentifiers(new String[]{"Student ID", "Date", "Status"});

        table = new JTable(model);
        JScrollPane scrollPane = new JScrollPane(table);
        add(scrollPane, BorderLayout.CENTER);

        loadAttendanceData();

        JButton backButton = new JButton("Back");
        backButton.addActionListener(e -> {
            this.dispose();
            new AdminDashboard().setVisible(true);
        });
        add(backButton, BorderLayout.SOUTH);
    }

    private void loadAttendanceData() {
        model.setRowCount(0); // Clear any previous data
        try {
            File file = new File("E:/School Managment System/CppBackend/attendance.txt");
            if (!file.exists()) {
                JOptionPane.showMessageDialog(this, "attendance.txt not found!");
                return;
            }

            Scanner reader = new Scanner(file);
            while (reader.hasNextLine()) {
                String line = reader.nextLine().trim();
                if (!line.isEmpty()) {
                    String[] data = line.split(",");
                    if (data.length == 3) {
                        model.addRow(new Object[]{data[0], data[1], data[2]});
                    }
                }
            }
            reader.close();
        } catch (Exception e) {
            JOptionPane.showMessageDialog(this, "Error loading attendance: " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            new ViewAttendanceForm().setVisible(true);
        });
    }
}
