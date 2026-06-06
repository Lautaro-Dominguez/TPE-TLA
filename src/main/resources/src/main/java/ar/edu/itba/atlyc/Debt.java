package ar.edu.itba.atlyc;

import java.time.LocalDate;
import java.util.Currency;

public class Debt extends EconomicVariable{
    private String category;
    private LocalDate deadline;
    private double interest;
    private double minPayment;

    public Debt(double value, Currency currency, String category, LocalDate deadline, double interest, double minPayment, String name){
        super(value,currency, name);
        this.category = category;
        this.deadline = deadline;
        this.interest = interest;
        this.minPayment = minPayment;
    }

    public void setCategory(String category){
        this.category = category;
    }

    public String getCategory(){
        return category;
    }

    public String plan(int cuotas){
        StringBuilder ans = new StringBuilder();
        ans.append("Cuota;Pago\n");
        double pending = getValue();
        double cuotasPending = cuotas;
        double pay = Math.max(minPayment, pending/cuotasPending);
        ans.append("1;%.2f\n".formatted(pay));
        pending -= pay;
        pending += pending * interest;
        cuotasPending--;
        for(int i = 1; i < cuotas; i++){
            pay = pending/cuotasPending;
            ans.append("%d;%.2f\n".formatted(i+1, pay));
            pending -= pay;
            pending += pending * interest;
        }
        return ans.toString();
    }

    @Override
    public String toString(){
        return "%.2f;%s;%s;%s;%.2f;%.2f".formatted(getValue(), getCurrency(), getCategory(), deadline, interest, minPayment);
    }
}
