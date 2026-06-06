package ar.edu.itba.atlyc;

import org.knowm.xchart.PieChart;
import org.knowm.xchart.PieChartBuilder;
import org.knowm.xchart.SwingWrapper;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.time.LocalDate;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class UtilFunctions {

    public static String getMaxCategory(List<Expense> l){
        if(l.equals(Collections.emptyList())){
            return null;
        }
        double max = 0;
        String category = null;
        for(Expense e : l){
            if(e.getValue() > max){
                max = e.getValue();
                category = e.getCategory();
            }
        }
        return category;
    }

    public static String getMinCategory(List<Expense> l){
        if(l.equals(Collections.emptyList())){
            return null;
        }
        double max = l.getFirst().getValue();
        String category = l.getFirst().getCategory();
        for(Expense e : l){
            if(e.getValue() < max){
                max = e.getValue();
                category = e.getCategory();
            }
        }
        return category;
    }

    public static double getTotalIncome(List<Income> l, LocalDate from, LocalDate up){
        double total = 0;
        if(from.isAfter(up)){
            throw new IllegalArgumentException();
        }
        for (Income e : l) {
            LocalDate cursor = e.getCreationDate();
            while (cursor.isBefore(from)) {
                cursor = cursor.plus(e.getPeriodicity());
            }
            while (!cursor.isAfter(up)) {
                total += e.getValue();
                cursor = cursor.plus(e.getPeriodicity());
            }
        }
        return total;
    }

    public static double getTotalIncome(List<Income> l){
        double total = 0;
        for (Income e : l) {
            total += e.getValue();
        }
        return total;
    }


    public static double getTotalExpense(List<Expense> l, LocalDate from, LocalDate up){
        double total = 0;
        if(from.isAfter(up)){
            throw new IllegalArgumentException();
        }
        for (Expense e : l) {
            LocalDate cursor = e.getCreationDate();
            while (cursor.isBefore(from)) {
                cursor = cursor.plus(e.getPeriodicity());
            }
            while (!cursor.isAfter(up)) {
                total += e.getValue();
                cursor = cursor.plus(e.getPeriodicity());
            }
        }
        return total;
    }

    public static double getTotalExpense(List<Expense> l){
        double total = 0;
        for (Expense e : l) {
            total += e.getValue();
        }
        return total;
    }

    public static double getTotalExpense(List<Expense> l, LocalDate from, LocalDate up, String category){
        double total = 0;
        if(from.isAfter(up)){
            throw new IllegalArgumentException();
        }
        if(category==null){
            getTotalExpense(l, from, up);
        }
        for (Expense e : l) {
            if(e.getCategory().equals(category)){
                LocalDate cursor = e.getCreationDate();
                while (cursor.isBefore(from)) {
                    cursor = cursor.plus(e.getPeriodicity());
                }
                while (!cursor.isAfter(up)) {
                    total += e.getValue();
                    cursor = cursor.plus(e.getPeriodicity());
                }
            }
        }
        return total;
    }

    public static double getTotalExpense(List<Expense> l, String category){
        double total = 0;
        if(category==null){
            getTotalExpense(l);
        }
        for (Expense e : l) {
            if(e.getCategory().equals(category)){
                total += e.getValue();
            }
        }
        return total;
    }

    public static void export(List<EconomicVariable> l){
        try(PrintWriter file = new PrintWriter("output-%s.csv".formatted(LocalDate.now()))){
            for(EconomicVariable e : l){
                file.println(e.toString());
            }
        }
        catch (FileNotFoundException e){
            throw new RuntimeException();
        }
    }

    public static void export(EconomicVariable e){
        try(PrintWriter file = new PrintWriter("output-%s.csv".formatted(LocalDate.now()))){
            file.println(e.toString());
        }
        catch (FileNotFoundException ex){
            throw new RuntimeException();
        }
    }


    public static void circleGraphic(List<? extends EconomicVariable> l){
        PieChart chart = new PieChartBuilder()
                .width(800)
                .height(600)
                .title("Data")
                .build();
        for(EconomicVariable e : l){
            chart.addSeries(e.getName(), e.getValue());
        }
        new SwingWrapper<>(chart).displayChart();
    }

    public static List<EconomicVariable> balance(LocalDate from, LocalDate up, List<List<? extends EconomicVariable>> data){
        List<EconomicVariable> ans = new ArrayList<>();
        for(List<? extends EconomicVariable> l : data){
            for(EconomicVariable e : l){
                if(e instanceof SoftEconomicVariable){
                    SoftEconomicVariable se = (SoftEconomicVariable) e;
                    LocalDate cursor = e.getCreationDate();
                    while (cursor.isBefore(from)) {
                        cursor = cursor.plus(se.getPeriodicity());
                    }
                    while (!cursor.isAfter(up)) {
                        ans.add(e);
                        cursor = cursor.plus(se.getPeriodicity());
                    }
                }else{
                    ans.add(e);
                }
            }
        }
        return ans;
    }
}
