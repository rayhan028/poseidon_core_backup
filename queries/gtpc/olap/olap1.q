auto timePeriod = [&](auto res) {
    auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
    auto gdt = %max_ship_dt - hours(24 * %delta);
    return dt <= gdt;
});

Sort([$0:int ASC],
    GroupBy([$0], 
            [sum($1:int), sum($2:double), avg($1:int), avg($2:double), count($0:uint64)],
        Project([year($0.number:int), $0.quantity:int, $0.amount:double],
            Filter(timePeriod($0.delivery_d:datetime),
                NodeScan("OrderLine")
            )
        )
    )
)