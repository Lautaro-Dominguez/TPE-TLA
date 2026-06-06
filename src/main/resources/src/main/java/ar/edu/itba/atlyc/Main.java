package ar.edu.itba.atlyc;

import java.time.LocalDate;
import java.time.Period;
import java.util.ArrayList;
import java.util.Currency;
import java.util.List;

public class Main {
    public static void main(String[] args){
        List<Income> incomes = new ArrayList<>();
        List<Expense> expenses = new ArrayList<>();
        List<Goal> goals = new ArrayList<>();
        List<Debt> debts = new ArrayList<>();
        List<Asset> assets = new ArrayList<>();

        //DESDE ACA ES DONDE SE ESCRIBIRIA LO QUE MI PROGRAMA DICE

        //Creacion de un income:
        Income sueldo = new Income(15000.00, Currency.getInstance("ARS"), Period.ofMonths(1), "sueldo");
        incomes.add(sueldo);

        //Creacion de un expense:
        Expense facturaDeEdenor = new Expense(5000.00, Currency.getInstance("ARS"), Period.ofMonths(1), "Facturas", "facturaDeEdenor");
        expenses.add(facturaDeEdenor);

        //Creacion de un expense:
        Expense facturaDeAysa = new Expense(5000.00, Currency.getInstance("ARS"), Period.ofMonths(1), "Facturas", "facturaDeAysa");
        expenses.add(facturaDeAysa);

        //Creacion de un goal:
        Goal plataParaNuevaCompu = new Goal(50000.00, Currency.getInstance("ARS"), LocalDate.of(2026,07,30), "plataParaNuevaCompu");
        goals.add(plataParaNuevaCompu);

        //Creacion de una deuda:
        Debt deudaConEdenor = new Debt(100000.00, Currency.getInstance("ARS"), "Facturas", LocalDate.of(2026,05,05), 0.05, 20000, "deudaConEdenor");
        debts.add(deudaConEdenor);

        //Creacion de un asset:
        Asset auto = new Asset(30000, Currency.getInstance("USD"), "auto");
        assets.add(auto);

        //totalIncome as { from="01-01-2022", to="31-12-2028" };
        System.out.println(UtilFunctions.getTotalIncome(incomes, LocalDate.of(2020,01,01), LocalDate.of(2026, 12,31)));

        //totalIncome
        System.out.println(UtilFunctions.getTotalIncome(incomes));

        //totalExpenses as { from="01-01-2022", to="31-12-2028" };
        System.out.println(UtilFunctions.getTotalExpense(expenses, LocalDate.of(2020,01,01), LocalDate.of(2026, 12,31)));

        //totalExpenses
        System.out.println(UtilFunctions.getTotalExpense(expenses));

        //totalExpenses as { from="01-01-2022", to="31-12-2028", category="Facturas" };
        System.out.println(UtilFunctions.getTotalExpense(expenses, LocalDate.of(2020,01,01), LocalDate.of(2026, 12,31), "Facturas"));

        //maxCategory
        System.out.println(UtilFunctions.getMaxCategory(expenses));

        //minCategory
        System.out.println(UtilFunctions.getMinCategory(expenses));

        /*derivatedData available = totalIncome as {
            from "21-01-2026",
                    up "21-12-2026"
        } - totalExpenses as{
            from "21-01-2026",
                    up "21-12-2026"
        };*/
        double available = UtilFunctions.getTotalIncome(incomes, LocalDate.of(2026, 01, 21), LocalDate.of(2026, 12, 21))
                - UtilFunctions.getTotalExpense(expenses, LocalDate.of(2026, 01, 21), LocalDate.of(2026, 12, 21));
        System.out.println(available);

        // exchange sueldo USD;
        sueldo.exchange(Currency.getInstance("USD"));

        //change-periodicity sueldo yearly;
        sueldo.changePeriodicity(Period.ofYears(1));

        //export sueldo;
        UtilFunctions.export(sueldo);

        //plan for deudaConEdenor in 6;
        System.out.println(deudaConEdenor.plan(6));

        //balance balanceDe2025 as{
        //    from "01-01-2025",
        //    up "31-12-2025"
        //};
        //export balance;
        List<List<? extends EconomicVariable>> variables = new ArrayList<>();
        variables.add(incomes);
        variables.add(expenses);
        variables.add(goals);
        variables.add(debts);
        variables.add(assets);
        List<EconomicVariable> balance = UtilFunctions.balance(LocalDate.of(2026, 01, 01), LocalDate.of(2026,12,31), variables);
        UtilFunctions.export(balance);

        //circleGraphic totalExpenses
        UtilFunctions.circleGraphic(expenses);

        //exchange sueldo USD;
        sueldo.exchange(Currency.getInstance("USD"));
        System.out.println(sueldo);

        //exchange auto ARS;
        auto.exchange(Currency.getInstance("ARS"));
        System.out.println(auto);
    }
}
